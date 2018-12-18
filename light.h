#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include "shaders.h"
#include "entity.h"

class Light: public Entity
{
public:
	enum Kind { Point, Spot, Directional };

	glm::vec3 color;
	float radius;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	float constant, linear, quadratic;
	float cutoff, outer_cutoff; // spotlight ring thickness

	Kind kind;
	
	Light();
	~Light();

	void setUniforms(Shaders shader, int idx);
	glm::vec3 getDirecton();
};

// GLSL representation of the Light object
#define LIGHT_GLSL "\n\
struct Light {\n\
	vec3 position;\n\
	vec3 direction;\n\
	vec3 ambient;\n\
	vec3 diffuse;\n\
	vec3 specular;\n\
	vec3 attenuation;\n\
	vec2 spot;\n\
	vec3 attributes;\n\
};\n";