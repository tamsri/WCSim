#include "transmitter.hpp"

#include <iostream>

#include "ray.hpp"
#include "camera.hpp"
#include "radiation_pattern.hpp"
#include "record.hpp"
#include "object.hpp"

#include <math.h>
#include <glm/gtc/matrix_transform.hpp>


//transform_ = transform;
//glm::mat4 trans;
//const glm::vec4 direction = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
//for (int i = 0; i < 360; i+=10) {
//	for (int j = 0; j < 360; j+=1) {
//		trans = glm::rotate(glm::mat4(1.0f), glm::radians((float)j) , glm::vec3(0.0f, 1.0f, 0.0f));
//		trans = glm::rotate(trans, glm::radians((float)j), glm::vec3(0.0f, 0.0f, 1.0f));
//		auto new_direction = trans * direction;
//		Ray* ray = new Ray(transform.position, glm::vec3(new_direction.x, new_direction.y, new_direction.z));
//		ray->InitializeRay((i)/720.0f);
//		//std::cout << " is " << j << std::endl;
//		rays_.push_back(ray);
//	}
//}

Transmitter::Transmitter(Transform transform,
						float frequency, 
						RayTracer * ray_tracer):	transform_(transform), 
													frequency_(frequency),
													ray_tracer_(ray_tracer)
{
}

void Transmitter::DrawRadiationPattern(Camera * camera)
{
	for (auto & ray : rays_) {
		ray->DrawObject(camera);
	}
}

void Transmitter::AssignRadiationPattern(RadiationPattern* pattern)
{
	current_pattern = pattern;
	rays_.clear();
	for (auto angles : current_pattern->pattern_) {
		float theta = angles.first.first;
		float phi = angles.first.second;
		float gain = angles.second;

		glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(theta), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(phi), glm::vec3(0.0f, 1.0f, .0f));
		auto new_direction = glm::normalize(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // todo implement later
		Ray* ray = new Ray(transform_.position, glm::vec3(new_direction.x, new_direction.y, new_direction.z));
		
		/*if (gain < 0.0f)
			ray->InitializeRay(0.0f);
		else*/
		ray->InitializeRay(pow(10,gain/10.0f));
		//std::cout << " is " << j << std::endl;
		rays_.push_back(ray);
	};
}

float Transmitter::GetFrequency()
{
	return frequency_;
}

Point* Transmitter::GetPoint()
{
	return current_point_;
}
