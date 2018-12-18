#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>


/* // FIXME this barks on linking
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION
*/

#include "tiny_gltf.h"
#include "entity.h"
#include "shaders.h"

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

bool loadModel(tinygltf::Model &model, const char* filename, bool is_binary);
void bindModelNodes(std::map<int, GLuint> vbos, tinygltf::Model &model, tinygltf::Node &node);

class Model: public Entity
{
	GLuint vao;
	std::vector<GLuint> textures;
	tinygltf::Model model;

	void drawModelNodes(tinygltf::Node &node, Shaders &shader);
	void drawMesh(tinygltf::Mesh &mesh, Shaders &shader);
	void bindModel();
	void bindTextures();

public:
	const char* filename;

	Model(const char* filename, bool is_binary);
	~Model();

	void draw(Shaders &shader);
};