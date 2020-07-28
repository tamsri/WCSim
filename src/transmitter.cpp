#include "transmitter.hpp"

#include "ray.hpp"
#include "camera.hpp"

#include <math.h>
#include <glm/gtc/matrix_transform.hpp>

Transmitter::Transmitter(Transform transform, Shader * shader)
{
	transform_ = transform;
	glm::mat4 trans;
	const glm::vec4 direction = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
	for (int i = 0; i < 360; i+=30) {
		for (int j = 0; j < 360; j+=30) {
			trans = glm::rotate(glm::mat4(1.0f), (float)i , glm::vec3(1.0f, 0.0f, 0.0f));
			trans = glm::rotate(trans, (float)j, glm::vec3(0.0f, 1.0f, 0.0f));
			auto new_direction = trans * direction;
			Ray* ray = new Ray(transform.position, glm::vec3(new_direction.x, new_direction.y, new_direction.z), shader);
			ray->InitializeRay(1);
			rays_.push_back(ray);
		}
	}

}

void Transmitter::DrawRadiationPattern(Camera * camera)
{
	for (auto & ray : rays_) {
		ray->DrawObject(camera);
	}
}
