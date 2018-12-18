#include "camera.h"


Camera::Camera(float fov)
{
	this->fov = fov;
}


Camera::~Camera()
{
}

void Camera::update(Window &window)
{
	GLint w, h;
	glfwGetWindowSize(window.window, &w, &h);

	if ((w > 0) && (h > 0))
		this->projection = glm::perspective(glm::radians(this->fov), 
			(float)w / (float)h, this->near, this->far);
}


glm::mat4 Camera::genMVP(glm::mat4 &model_mat)
{
	glm::mat4 mvp = this->projection * this->rotation * model_mat;

	return mvp;
}