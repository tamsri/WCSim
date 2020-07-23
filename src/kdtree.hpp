#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

#include <glm/glm.hpp>

class Triangle;
class Ray;

class KDNode {
public:
	KDNode(int axis, const std::vector<const Triangle*>& objects);
	~KDNode();

	int GetAxis();
	KDNode* GetChildren(unsigned int index);
	void SetChildren(unsigned int index, KDNode * node);
	float GetPivot();
private:
	int axis_;
	KDNode* children_[2];
	std::vector<const Triangle*> objects_;
	std::vector<glm::vec3> corners_;
	float pivot_;

};

class KDTree {

public:
	KDTree(const std::vector<const Triangle*>& objects);
	~KDTree();
	bool IsHit(const Ray & ray) const;
private:
	KDNode * root_;
	KDNode* Build(const std::vector<const Triangle*>& objects, int depth) const;
	bool IsHit(const Ray& ray, KDNode * node, bool is_hit, int depth) const;
};

#endif // !KDTREE_H