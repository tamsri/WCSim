#ifndef KDTREE_H
#define KDTREE_H

#include <vector>

#include <glm/glm.hpp>

class Triangle;
class Ray;
class HitRecord;

class KDNode {
public:
	KDNode(int axis, const std::vector<const Triangle*>& objects);
	~KDNode();

	int GetAxis();
	KDNode* GetChildren(unsigned int index);
	void SetChildren(unsigned int index, KDNode * node);
	float GetPivot();
	glm::vec3 GetCorner(unsigned int index);

	std::vector<const Triangle*> objects_;

private:
	int axis_;
	KDNode* children_[2];
	std::vector<glm::vec3> corners_;
	float pivot_;

};

class KDTree {

public:
	KDTree(const std::vector<const Triangle*>& objects);
	~KDTree();
	bool IsHit(const Ray & ray, float & t) const;
private:
	KDNode * root_;
	KDNode* Build(const std::vector<const Triangle*>& objects, int depth) const;
	bool IsHit(const Ray& ray, KDNode * node, float & t, int depth) const;
};

#endif // !KDTREE_H