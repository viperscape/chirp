#include "entity.h"



Entity::Entity()
{
	this->position = glm::vec3(0, 0, 0);
	this->rotation = glm::mat4(1.0f);
	this->enabled = true;
	this->lookAt(0,0,-1);
}


Entity::~Entity()
{
}

void Entity::lookAt(float x, float y, float z)
{
	glm::vec3 dir = glm::vec3(x,y,z);
	this->lookAt(dir);
}
void Entity::lookAt(glm::vec3 &lookat)
{
	this->rotation = glm::lookAt(
		this->position,
		lookat,
		glm::vec3(0, 1, 0)
	);
}