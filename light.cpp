#include "light.h"

#include <math.h>
#include <string>

Light::Light()
{
	this->position = glm::vec3(0, 0, 0);

	this->radius = 10;

	this->ambient = glm::vec3(0.3, 0.3, 0.3);
	this->diffuse = glm::vec3(0.8, 0.8, 0.8);
	this->specular = glm::vec3(1.0, 1.0, 1.0);

	this->constant = 1.0;
	this->linear = 0.09f;
	this->quadratic = 0.032f;

	this->kind = Light::Point;
	this->enabled = true;
}


Light::~Light()
{
}

glm::vec3 Light::getDirecton()
{
	return glm::vec3(this->rotation[0][2], this->rotation[1][2], this->rotation[2][2]);
}

void Light::setUniforms(Shaders shader, int idx)
{
	const std::string prefix = "lights[" + std::to_string(idx) + "].";

	GLuint uid;

	// we build a vec3 for reduced calls
	uid = shader.get((prefix + "attributes").c_str());
	glm::vec3 v = glm::vec3(0, 0, 0);
	v.x = (float)this->kind;
	v.y = (float)this->enabled;
	//v.z = (float)this->radius;
	shader.set3f(uid, v);

	if (!this->enabled) return; // skip rest of uniforms if disabled

	if (this->kind != Light::Directional)
	{
		uid = shader.get((prefix + "position").c_str());
		shader.set3f(uid, this->position);
	}

	if ((this->kind == Light::Directional) ||
		(this->kind == Light::Spot))
	{
		uid = shader.get((prefix + "direction").c_str());
		glm::vec3 dir = this->getDirecton();
		shader.set3f(uid, dir);
	}

	uid = shader.get((prefix + "ambient").c_str());
	shader.set3f(uid, this->ambient);

	uid = shader.get((prefix + "diffuse").c_str());
	shader.set3f(uid, this->diffuse);

	uid = shader.get((prefix + "specular").c_str());
	shader.set3f(uid, this->specular);


	uid = shader.get((prefix + "attenuation").c_str());
	v = glm::vec3(0, 0, 0);
	v.x = (float)this->constant;
	v.y = (float)this->linear;
	v.z = (float)this->quadratic;
	shader.set3f(uid, v);

	uid = shader.get((prefix + "spot").c_str());
	glm::vec2 spot = glm::vec3(0, 0, 0);
	spot.x = (float)this->cutoff;
	spot.y = (float)this->outer_cutoff;
	shader.set2f(uid, spot);
}