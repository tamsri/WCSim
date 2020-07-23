#include "kdtree.hpp"

#include "triangle.hpp"

KDTree::KDTree(const std::vector<const Triangle*>& objects)
{
    root_ = Build(objects, 0);
}

KDTree::~KDTree()
{
    delete root_;
}

bool KDTree::IsHit(const Ray& ray) const
{
    return IsHit(ray, root_, false, 0);
}

bool KDTree::IsHit(const Ray& ray, KDNode* node, bool is_hit, int depth) const
{
    return false;
}

KDNode* KDTree::Build(const std::vector<const Triangle*>& objects, int depth) const
{
    KDNode* node = new KDNode(depth & 3, objects);
    if (objects.size() > 128 && depth < 8) {
        std::vector<const Triangle*> left, right;
        for (auto & object : objects) {
            for (auto i = 0; i < 3; ++i) {
                if (object->GetPoints()[i][node->GetAxis()] <= node->GetPivot()) {
                    left.emplace_back(object);
                    break;
                }
            }
            for (auto i = 0; i < 3; ++i) {
                if (object->GetPoints()[i][node->GetAxis()] <= node->GetPivot()) {
                    right.emplace_back(object);
                    break;
                }
            
            }
        }
        auto common = left.size() + right.size() - objects.size();
        if (common * 4 < objects.size()) {
            node->SetChildren(0, Build(left, depth + 1));
            node->SetChildren(1, Build(right, depth + 1));
        }
    }
    return node;
}

KDNode::KDNode(int axis, const std::vector<const Triangle*>& objects)
{
    axis_ = axis;
    children_[0] = nullptr;
    children_[1] = nullptr;
    objects_ = objects;
    corners_ = {
        std::numeric_limits<float>::max() * glm::vec3(1.0f,1.0f,1.0f),
        std::numeric_limits<float>::min() * glm::vec3(1.0f,1.0f,1.0f)
    };
    pivot_ = 0;

    if (objects_.empty()) return;
    // Determine corners
    for (auto& object : objects_) {
        for (auto& vertex : object->GetPoints()) {
            pivot_ += vertex[axis] / (objects.size() * 3);
            for (auto i = 0; i < 3; ++i) {
                if (vertex[i] < corners_[0][i]) corners_[0][i] = vertex[i];
                if (vertex[i] > corners_[1][i]) corners_[1][i] = vertex[i];
            }
        }
    }
}

KDNode::~KDNode()
{
    delete children_[0];
    delete children_[1];
}

int KDNode::GetAxis()
{
    return axis_;
}

KDNode* KDNode::GetChildren(unsigned int i)
{
    return children_[i];
}

void KDNode::SetChildren(unsigned int index, KDNode * node)
{
    children_[index] = node;
}

float KDNode::GetPivot()
{
    return pivot_;
}
