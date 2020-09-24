#include "kdtree.hpp"

#include<iostream>

#include "triangle.hpp"
#include "ray.hpp"


KDTree::KDTree(const std::vector<const Triangle*>& objects)
{
    root_ = Build(objects, 0);
}

KDTree::~KDTree()
{
    delete root_;
}

bool KDTree::IsClosestHit(const Ray& ray, float & t) const
{
    t = std::numeric_limits<float>::max(); // Initialize t before checking
    return IsClosestHit(ray, root_, t, 0);
}

bool KDTree::IsClosestHit(const Ray& ray, KDNode* node, float & t, int depth) const
{
    glm::vec3 direction = ray.GetDirection();
    glm::vec3 origin = ray.GetOrigin();
    glm::vec3 point_1 = node->GetCorner(0);
    glm::vec3 point_2 = node->GetCorner(1);

    float distances[6] = {
      direction.x ? (point_1.x - origin.x) / direction.x : 0,
      direction.x ? (point_2.x - origin.x) / direction.x : 0,
      direction.y ? (point_1.y - origin.y) / direction.y : 0,
      direction.y ? (point_2.y - origin.y) / direction.y : 0,
      direction.z ? (point_1.z - origin.z) / direction.z : 0,
      direction.z ? (point_2.z - origin.z) / direction.z : 0
    };
    auto distance = std::numeric_limits<float>::max();
    constexpr float k_epsilon = 0.000001f;
    for (int i = 0; i < 6; ++i) {
        if (distances[i] == 0) continue;
        glm::vec3 point = origin + distances[i] * direction;
        if (point.x < point_1.x - k_epsilon || point.x > point_2.x + k_epsilon) continue;
        if (point.y < point_1.y - k_epsilon || point.y > point_2.y + k_epsilon) continue;
        if (point.z < point_1.z - k_epsilon || point.z > point_2.z + k_epsilon) continue;

        if (distances[i] < distance) distance = distances[i];
        //std::cout << t  << std::endl;
        if (distance >= t) return false;
     
        if (node->GetChildren(0) && node->GetChildren(1)) {
            auto point = ray.GetOrigin() + ray.GetDirection() * t;
            auto prefer = point[node->GetAxis()] <= node->GetPivot() ? 0 : 1;
            for (auto i : { prefer, prefer ^ 1 }) {
                float temp_t;
                if (IsClosestHit(ray, node->GetChildren(i), temp_t, depth + 1) 
                    && temp_t < t) 
                    t = temp_t;
            }
        }
        else {
            for (auto& triangle : node->triangles_) {
                float temp_t;
                if (triangle->IsHit(ray, temp_t) 
                    && temp_t < t) t = temp_t;
            }
        }
    }
    return true;
}

bool KDTree::IsSomeHit(const Ray& ray, std::set<std::pair<float, Triangle *>>& hit_triangles) const
{
    return IsSomeHit(ray, root_, hit_triangles, 0);
}

bool KDTree::IsSomeHit(const Ray& ray, KDNode* node, std::set<std::pair<float, Triangle*>> & hit_triangles, int depth) const
{
    glm::vec3 direction = ray.GetDirection();
    glm::vec3 origin = ray.GetOrigin();
    glm::vec3 point_1 = node->GetCorner(0);
    glm::vec3 point_2 = node->GetCorner(1);

    float distances[6] = {
      direction.x ? (point_1.x - origin.x) / direction.x : 0,
      direction.x ? (point_2.x - origin.x) / direction.x : 0,
      direction.y ? (point_1.y - origin.y) / direction.y : 0,
      direction.y ? (point_2.y - origin.y) / direction.y : 0,
      direction.z ? (point_1.z - origin.z) / direction.z : 0,
      direction.z ? (point_2.z - origin.z) / direction.z : 0
    };
    constexpr float k_epsilon = 0.000001f;
    for (int i = 0; i < 6; ++i) {
        if (distances[i] == 0) continue;
        glm::vec3 point = origin + distances[i] * direction;
        if (point.x < point_1.x || point.x > point_2.x + k_epsilon) continue;
        if (point.y < point_1.y || point.y > point_2.y + k_epsilon) continue;
        if (point.z < point_1.z || point.z > point_2.z + k_epsilon) continue;


        if (node->GetChildren(0) && node->GetChildren(1)) {
            float t = (hit_triangles.empty())? 0.0f:(hit_triangles.end())->first;
            auto point = ray.GetOrigin() + ray.GetDirection() * t;
            auto prefer = point[node->GetAxis()] <= node->GetPivot() ? 0 : 1;
            for (auto i : { prefer, prefer ^ 1 }) {
                IsSomeHit(ray, node->GetChildren(i), hit_triangles, depth + 1);
            }
        }
        else {
            for (auto& triangle : node->triangles_) {
                float temp_t;
                Triangle* hit_triangle;
                if (triangle->IsHit(ray, temp_t, hit_triangle))
                {
                    std::pair<float, Triangle*> hit_pair = { temp_t , hit_triangle };
                    hit_triangles.insert(hit_pair);
                }
            }
        }
    }
    return hit_triangles.size() != 0;
}

KDNode* KDTree::Build(const std::vector<const Triangle*>& objects, int depth) const
{
    KDNode* node = new KDNode(depth % 3, objects);
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
    triangles_ = objects;
    corners_ = {
        std::numeric_limits<float>::max() * glm::vec3(1.0f,1.0f,1.0f),
        std::numeric_limits<float>::min() * glm::vec3(1.0f,1.0f,1.0f)
    };
    pivot_ = 0.0f;
    if (!triangles_.empty()) return;
    // Determine corners
    for (auto& triangle : triangles_) {
        for (auto& vertex : triangle->GetPoints()) {
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

glm::vec3 KDNode::GetCorner(unsigned int index)
{
    return glm::vec3(corners_[index]);
}

float KDNode::GetPivot()
{
    return pivot_;
}