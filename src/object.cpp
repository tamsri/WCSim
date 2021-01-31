#include "object.hpp"

#include "camera.hpp"
#include "shader.hpp"
Shader* Object::default_shader_;
Shader* Object::ray_shader_;

void Object::DrawObject(Camera* camera) const
{
	if (shader_ == nullptr) /// todo implement without if (if it could be faster)
	{
		default_shader_->Use();
		default_shader_->SetMat4("projection", camera->projection_);
		default_shader_->SetMat4("view", camera->view_);
		default_shader_->SetMat4("model", model_);
    }
	else {
		shader_->Use();
		shader_->SetMat4("projection", camera->projection_);
		shader_->SetMat4("view", camera->view_);
		shader_->SetMat4("model", model_);
		shader_->SetVec4("input_color", color_);
	}
	Draw();
}

void Object::MoveTo(glm::vec3 position)
{
	transform_.position = position;
	model_ = glm::translate(glm::mat4(1.0f), transform_.position);
	model_ = glm::scale(model_, transform_.scale);
}

void Object::TransformTo(Transform transform)
{
	transform_ = transform;
	model_ = glm::translate(glm::mat4(1.0f), transform_.position);
	//model_ = glm::scale(model_, transform.scale);
	model_ = glm::rotate(model_, transform.rotation.x, glm::vec3{ 0.0, -1.0, 0.0 });
	model_ = glm::rotate(model_, transform.rotation.y, glm::vec3{ 0.0, 0.0, -1.0 });
}
