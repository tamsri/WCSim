#include "object.hpp"

#include "camera.hpp"
#include "shader.hpp"

Shader* Object::default_shader_;


void Object::DrawObject(Camera* camera) const
{
	if (shader_ == nullptr) /// todo implement without if(it could be faster)
	{
		default_shader_->SetMat4("projection", camera->projection_);
		default_shader_->SetMat4("view", camera->view_);
		default_shader_->SetMat4("model", model_);
		default_shader_->Use();
	}
	else {
		shader_->SetMat4("projection", camera->projection_);
		shader_->SetMat4("view", camera->view_);
		shader_->SetMat4("model", model_);
		shader_->Use();
	}
	Draw();
}
