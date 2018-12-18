#pragma once

#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

class Shaders
{
public:
	GLuint pid;
	const char* filename;
	std::string glsl_preludes; // generated code can be inserted in shader before compilation

	Shaders(const char* filename, const std::string &preludes);
	~Shaders();
	void compile();

	GLuint get(const char* name);
	void set3f(GLuint uid, glm::vec3 &vec);
	void set4f(GLuint uid, float *vec);
	void set2f(GLuint uid, glm::vec2 &vec);
	void set3x3f(GLuint uid, glm::mat3 &mat);
	void set4x4f(GLuint uid, glm::mat4 &mat);
};

