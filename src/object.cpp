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
