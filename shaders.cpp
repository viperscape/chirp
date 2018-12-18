#include "shaders.h"
#include <iostream>
#include <vector>
#include <fstream>

Shaders::Shaders(const char* filename, const std::string &preludes)
{
	this->glsl_preludes += "#version 330 core\n";
	this->glsl_preludes += preludes;
	this->filename = filename;
	this->compile();
}

void Shaders::compile()
{
	if (this->pid > 0) glDeleteProgram(this->pid); // recompiling?

	std::string shader_file = this->filename;
	std::ifstream ifs_frag(shader_file + ".frag");
	std::ifstream ifs_vert(shader_file + ".vert");


	// Create the shaders
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);


	GLint Result = GL_FALSE;
	int InfoLogLength;

	std::string str_vert((std::istreambuf_iterator<char>(ifs_vert)), std::istreambuf_iterator<char>());
	std::string str_shader = this->glsl_preludes + str_vert;
	char const* buf_vert = str_shader.c_str();
	// Compile Vertex Shader
	glShaderSource(VertexShaderID, 1, &buf_vert, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
		std::cout << buf_vert << std::endl;
	}


	std::string str_frag((std::istreambuf_iterator<char>(ifs_frag)), std::istreambuf_iterator<char>());
	str_shader = this->glsl_preludes + str_frag;
	char const* buf_frag = str_shader.c_str();
	// Compile Fragment Shader
	glShaderSource(FragmentShaderID, 1, &buf_frag, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
		std::cout << buf_frag << std::endl;
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	this->pid = ProgramID;
}


Shaders::~Shaders()
{
	glDeleteProgram(this->pid);
}

GLuint Shaders::get(const char* name)
{
	return glGetUniformLocation(this->pid, name);
}


void Shaders::set3f(GLuint uid, glm::vec3 &vec)
{
	glUniform3fv(uid, 1, &vec[0]);
}

void Shaders::set4f(GLuint uid, float *vec)
{
	glUniform4fv(uid, 1, &vec[0]);
}

void Shaders::set2f(GLuint uid, glm::vec2 &vec)
{
	glUniform2fv(uid, 1, &vec[0]);
}

void Shaders::set3x3f(GLuint uid, glm::mat3 &mat)
{
	glUniformMatrix3fv(uid, 1, GL_FALSE, &mat[0][0]);
}

void Shaders::set4x4f(GLuint uid, glm::mat4 &mat)
{
	glUniformMatrix4fv(uid, 1, GL_FALSE, &mat[0][0]);
}

