#include "object.hpp"

#include "camera.hpp"
#include "shader.hpp"

void Object::DrawObject(Camera* camera) const
{
	shader_->SetMat4("projection", camera->projection_);
	shader_->SetMat4("view", camera->view_);
	shader_->SetMat4("model", model_);
	shader_->Use();
	Draw();
}
