#ifndef KDTREE_H
#define KDTREE_H

#include <vector>
#include <set>

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

	std::vector<const Triangle*> triangles_;

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
	bool IsClosestHit(const Ray & ray, float & t) const;
	bool IsSomeHit(const Ray& ray, std::set<std::pair<float, Triangle*>>& hit_triangles) const;
private:
	KDNode * root_;
	KDNode* Build(const std::vector<const Triangle*>& objects, int depth) const;
	bool IsClosestHit(const Ray& ray, KDNode * node, float & t, int depth) const;
	bool IsSomeHit(const Ray& ray, KDNode* node, std::set<std::pair<float, Triangle*>>& hit_triangles, int depth) const;
};

/*class Intersection {
public:
	float hit_distance;
	glm::vec3 position;
	const Triangle * surface;

	static const Intersection MISS;
};*/
//const Intersection::MISS = { hit_distance = std::numberic_limit<float>::max() };
#endif // !KDTREE_H