#pragma once

#include <glm/gtc/matrix_transform.hpp>

class Entity
{
public:
	glm::vec3 position;
	glm::mat4 rotation;
	bool enabled;

	Entity();
	~Entity();

	void lookAt(float x, float y, float z);
	void lookAt(glm::vec3 &lookat);
};

