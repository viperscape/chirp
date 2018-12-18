#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "window.h"
#include "entity.h"

class Camera: public Entity
{
public:
	glm::mat4 projection;
	float fov;
	float near = 0.1f;
	float far = 1000.0f;

	Camera(float fov);
	~Camera();

	void update(Window &window);
	glm::mat4 genMVP(glm::mat4 &model_mat);
};

