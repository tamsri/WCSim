#ifndef RAY_H
#define RAY_H

#include "glm/glm.hpp"

#include "object.hpp"
class Shader;

class Ray : public Object{
public:
	Ray(glm::vec3 origin, glm::vec3 direction, Shader * shader);
	glm::vec3 GetOrigin() const;
	glm::vec3 GetDirection() const;
	
	//Visualisation
	void InitializeRay(float length);
	virtual void Draw() const;
private:
	glm::vec3 origin_;
	glm::vec3 direction_;

	//visualisation
	unsigned int vao_, vbo_;
};
#endif // !RAY_H