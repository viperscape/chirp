#include "model.h"

#include <iostream>

#include "shaders.h"

Model::Model(const char* filename, bool is_binary)
{
	this->filename = filename;
	if (!loadModel(this->model, filename, is_binary)) return;

	bindModel();
	this->bindTextures();
	//dbgModel(this->model); return;

	// Model matrix : an identity matrix (model will be at the origin)
	this->position = glm::vec3(0,0,0);
	this->rotation = glm::mat4(1.0f);
}


Model::~Model()
{
}


void Model::draw(Shaders &shader)
{
	if (this->vao > 0)
	{
		glBindVertexArray(this->vao);

		const tinygltf::Scene &scene = this->model.scenes[this->model.defaultScene];
		for (size_t i = 0; i < scene.nodes.size(); ++i) {
			drawModelNodes(this->model.nodes[scene.nodes[i]], shader);
		}

		glBindVertexArray(0);
	}
}

GLuint bindTexture(tinygltf::Image &image)
{
	GLuint texid;
	glGenTextures(1, &texid);

	glBindTexture(GL_TEXTURE_2D, texid);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2); // 2, 4 or 8 TODO check ext works on device

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width,
		image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
		&image.image.at(0));
	glGenerateMipmap(GL_TEXTURE_2D);

	return texid;
}

void Model::bindTextures()
{
	for (auto &texture : this->model.textures)
		this->textures.push_back(bindTexture(this->model.images[texture.source]));
}



void Model::drawMesh(tinygltf::Mesh &mesh, Shaders &shader)
{
	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		tinygltf::Primitive &primitive = mesh.primitives[i];
		tinygltf::Material &mat = this->model.materials[primitive.material];
		
		for (auto &value : mat.values)
		{
			if (value.first == "baseColorTexture")
			{
				glActiveTexture(GL_TEXTURE0 + 0);
				glBindTexture(GL_TEXTURE_2D, this->textures[value.second.TextureIndex()]);
			}
			else if (value.first == "metallicRoughnessTexture")
			{
				glActiveTexture(GL_TEXTURE0 + 2);
				glBindTexture(GL_TEXTURE_2D, this->textures[value.second.TextureIndex()]);
			}
			else if (value.first == "baseColorFactor")
			{
				std::vector<float> vec = { 
					(float)value.second.number_array[0],
					(float)value.second.number_array[1],
					(float)value.second.number_array[2],
					(float)value.second.number_array[3] 
				};
				shader.set4f(shader.get("base_color_factor"), &vec[0]);
			}
			else if (value.first == "metallicFactor")
			{
				glUniform1f(shader.get("metallic_factor"), value.second.number_value);
			}
			else if (value.first == "roughnessFactor")
			{
				glUniform1f(shader.get("roughness_factor"), value.second.number_value);
			}
		}

		for (auto &value : mat.additionalValues)
		{
			if (value.first == "normalTexture")
			{
				glActiveTexture(GL_TEXTURE0 + 1);
				glBindTexture(GL_TEXTURE_2D, this->textures[value.second.TextureIndex()]);
			}
			else if (value.first == "emissiveTexture") {}
		}


		tinygltf::Accessor indexAccessor = this->model.accessors[primitive.indices];
		int buffer_type = this->model.bufferViews[indexAccessor.bufferView].target;

		if (buffer_type == GL_ARRAY_BUFFER)
		{
			glDrawArrays(primitive.mode, indexAccessor.byteOffset, indexAccessor.count);
		}
		else
		{
			glDrawElements(primitive.mode, indexAccessor.count, indexAccessor.componentType,
				BUFFER_OFFSET(indexAccessor.byteOffset));
		}
	}
}

// recursively draw node and children nodes of model
void Model::drawModelNodes(tinygltf::Node &node, Shaders &shader)
{
	drawMesh(this->model.meshes[node.mesh], shader);
	for (size_t i = 0; i < node.children.size(); i++) {
		drawModelNodes(this->model.nodes[node.children[i]], shader);
	}
}

void Model::bindModel() {
	std::map<int, GLuint> vbos;
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);

	const tinygltf::Scene &scene = this->model.scenes[this->model.defaultScene];
	for (size_t i = 0; i < scene.nodes.size(); ++i) {
		bindModelNodes(vbos, this->model, this->model.nodes[scene.nodes[i]]);
	}

	glBindVertexArray(0);
	// cleanup vbos
	for (size_t i = 0; i < vbos.size(); ++i)
	{
		glDeleteBuffers(1, &vbos[i]);
	}
}



///

bool loadModel(tinygltf::Model &model, const char* filename, bool is_binary)
{
	tinygltf::TinyGLTF loader;
	std::string err;
	std::string warn;

	bool res;
	if (!is_binary)
		res = loader.LoadASCIIFromFile(&model, &err, &warn, filename);
	else
		res = loader.LoadBinaryFromFile(&model, &err, &warn, filename);

	if (!warn.empty()) {
		std::cout << "WARN: " << warn << std::endl;
	}

	if (!err.empty()) {
		std::cout << "ERR: " << err << std::endl;
	}

	if (!res)
		std::cout << "Failed to load glTF: " << filename << std::endl;
	else std::cout << "Loaded glTF: " << filename << std::endl;

	return res;
}



void bindMesh(std::map<int, GLuint> vbos, tinygltf::Model &model, tinygltf::Mesh &mesh)
{
	for (size_t i = 0; i < model.bufferViews.size(); ++i) {
		const tinygltf::BufferView &bufferView = model.bufferViews[i];
		if (bufferView.target == 0) { // TODO impl drawarrays
			std::cout << "WARN: bufferView.target is zero (need drawArrays)" << std::endl;
			//continue;  // Unsupported bufferView
			/*
			  From spec2.0 readme: https://github.com/KhronosGroup/glTF/tree/master/specification/2.0
				 ... drawArrays function should be used with a count equal to the count
				 property of any of the accessors referenced by the attributes property
				 (they are all equal for a given primitive).
			*/
		}

		tinygltf::Buffer buffer = model.buffers[bufferView.buffer];

		GLuint vbo;
		glGenBuffers(1, &vbo);
		vbos[i] = vbo;

		glBindBuffer(bufferView.target, vbo);
		glBufferData(bufferView.target, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);
	}

	for (size_t i = 0; i < mesh.primitives.size(); ++i)
	{
		tinygltf::Primitive primitive = mesh.primitives[i];

		tinygltf::Accessor indexAccessor = model.accessors[primitive.indices];

		for (auto& attrib : primitive.attributes)
		{
			tinygltf::Accessor accessor = model.accessors[attrib.second];
			int byteStride = accessor.ByteStride(model.bufferViews[accessor.bufferView]);
			glBindBuffer(model.bufferViews[accessor.bufferView].target, vbos[accessor.bufferView]);

			int size = 1;
			if (accessor.type != TINYGLTF_TYPE_SCALAR) {
				size = accessor.type;
			}

			int vaa = -1;
			if (attrib.first.compare("POSITION") == 0) vaa = 0;
			else if (attrib.first.compare("NORMAL") == 0) vaa = 1;
			else if (attrib.first.compare("TEXCOORD_0") == 0) vaa = 2;
			else if (attrib.first.compare("TANGENT") == 0) vaa = 3;

			if (vaa > -1)
			{
				glEnableVertexAttribArray(vaa);
				glVertexAttribPointer(
					vaa,
					size,
					accessor.componentType,
					accessor.normalized ? GL_TRUE : GL_FALSE,
					byteStride,
					BUFFER_OFFSET(accessor.byteOffset)
				);
			}
			else std::cout << "vaa missing: " << attrib.first << std::endl;
		}
	}
}

// bind models
void bindModelNodes(std::map<int, GLuint> vbos, tinygltf::Model &model, tinygltf::Node &node)
{
	bindMesh(vbos, model, model.meshes[node.mesh]);
	for (size_t i = 0; i < node.children.size(); i++) {
		bindModelNodes(vbos, model, model.nodes[node.children[i]]);
	}
}




void dbgModel(tinygltf::Model &model)
{
	for (auto &mesh : model.meshes)
	{
		std::cout << "mesh : " << mesh.name << std::endl;
		for (auto &primitive : mesh.primitives)
		{
			const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];

			std::cout << "indexaccessor: count " << indexAccessor.count << ", type " <<
				indexAccessor.componentType << std::endl;

			tinygltf::Material &mat = model.materials[primitive.material];
			for (auto &mats : mat.values)
			{
				std::cout << "mat : " << mats.first.c_str() << std::endl;
			}

			for (auto &image : model.images)
			{
				std::cout << "image name : " << image.uri << std::endl;
				std::cout << "  size : " << image.image.size() << std::endl;
				std::cout << "  w/h : " << image.width << "/" << image.height << std::endl;
			}

			std::cout << "indices : " << primitive.indices << std::endl;
			std::cout << "mode     : " << "(" << primitive.mode << ")" << std::endl;

			for (auto &attrib : primitive.attributes)
			{
				std::cout << "attribute : " << attrib.first.c_str() << std::endl;
			}
		}
	}
}

