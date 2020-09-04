#include "transmitter.hpp"

#include <iostream>

#include "ray.hpp"
#include "camera.hpp"
#include "record.hpp"
#include "object.hpp"

#include "radiation_pattern.hpp"
#include "ray_tracer.hpp"


#include <math.h>
#include <glm/gtc/matrix_transform.hpp>



Transmitter::Transmitter(Transform transform,
						float frequency, 
						RayTracer * ray_tracer):	transform_(transform), 
													frequency_(frequency),
													ray_tracer_(ray_tracer)
{
	current_pattern = nullptr;
	current_point_ = ray_tracer->InitializeOrCallPoint(transform_.position);
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
	int skipper = 0;
	for (auto angles : current_pattern->pattern_) {
		
		//if ((skipper++) % 50)continue;
		float theta = angles.first.first;
		float phi = angles.first.second;
		float gain = angles.second;
		//if(gain>0.0f) std::cout << "phi: " << phi << ", theta: " << theta << " value: " << gain << std::endl;
		glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(phi), glm::vec3(1.0f, 0.0f, 0.0f));
		trans = glm::rotate(trans, glm::radians(theta), glm::vec3(0.0f, 1.0f, .0f));
		auto new_direction = glm::normalize(trans * glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)); // todo implement later
		Ray* ray = new Ray(transform_.position, glm::vec3(new_direction.x, new_direction.y, new_direction.z));
		
		/*if (gain < 0.0f)
			ray->InitializeRay(0.0f);
		else*/
		//std::cout << gain + abs(pattern->min_gain_) << std::endl;
		//float normalized_gain = (gain)/(pattern->max_gain_ - pattern->min_gain_);
		float gain_lin = pow(10, gain / 10.f);
		float max_gain_lin = pow(10, pattern->max_gain_/10.0f);
		float min_gain_lin = pow(10, pattern->min_gain_ /10.0f);
		float normalized_gain = gain_lin / (max_gain_lin - min_gain_lin);
		
		if(normalized_gain >= 1e-3) ray->InitializeRay(normalized_gain*10.0f);
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
