#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "window.h"
#include "shaders.h"
#include "camera.h"
#include "light.h"



#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NOEXCEPTION
#define JSON_NOEXCEPTION

#include "model.h"


void displayLoop(Window& window)
{
	Camera camera = Camera(45.0f);
	//camera.position = glm::vec3(-2, 3, 3);

	std::vector<Light> lights = { Light(), Light() };
	lights[0].position = glm::vec3(100,100,-100);
	lights[0].lookAt(0, 0, 0);
	lights[0].kind = Light::Directional;
	lights[0].ambient = glm::vec3(0.1, 0.1, 0.1);
	lights[0].diffuse = glm::vec3(0.95, 0.84, 0.65);
	lights[0].specular = glm::vec3(0.3, 0.3, 0.3);
	//lights[0].enabled = false;

	lights[1].position = glm::vec3(5,0, 0);
	lights[1].diffuse = glm::vec3(0.2, 0.2, 0.8);
	//lights[1].enabled = false;

	std::string prelude = "#define MAX_LIGHTS " + std::to_string(lights.size()) + "\n";
	prelude += LIGHT_GLSL;

	Shaders shader = Shaders("shader_bdrf", prelude);
	glUseProgram(shader.pid);
	// setup normal sampler
	glUniform1i(shader.get("samp_norm"), 1);
	glUniform1i(shader.get("samp_metal"), 2);

	// grab uniforms to modify
	GLuint mvp_u = shader.get("MVP");
	GLuint mvi_u = shader.get("MVI");
	GLuint model_u = shader.get("model");
	GLuint camera_position_u = shader.get("camera_position");
	GLuint base_color_factor_u = shader.get("base_color_factor");

	//std::vector<Model> models = { Model("assets/rock/rock.glb", true) };
	//std::vector<Model> models = { Model("assets/fish/fish.gltf", false) };
	std::vector<Model> models = { Model("assets/rock/rock.gltf", false) };
	//std::vector<Model> models = { Model("assets/Avocado/glTF/Avocado.gltf", false) };

	models[0].position = glm::vec3(-1,1,-5); 
	//models[1].position = glm::vec3(0,-1,-5);

	while (!window.close())
	{
		window.resize();
		camera.update(window);
		camera.lookAt(models[0].position);

		glClearColor(0.2, 0.2, 0.2, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.set3f(camera_position_u, camera.position);

		// set uniforms for all lights
		for (int i = 0; i < lights.size(); ++i)
		{
			lights[i].setUniforms(shader, i);
		}

		for (auto &model : models)
		{
			for (int i = 0; i < lights.size(); ++i)
			{
				if (lights[i].kind == Light::Directional) continue;

				float light_dist = glm::distance(lights[i].position, model.position);
				float att = 1.0 / (lights[i].constant + lights[i].linear * light_dist +
					lights[i].quadratic * (light_dist * light_dist));

				if (att < 0.01)
				{
					bool was_enabled = lights[i].enabled;
					lights[i].enabled = false;
					lights[i].setUniforms(shader, i); // this is a quick op since we break after disabling uniform
					lights[i].enabled = was_enabled;
				}
			}


			glm::mat4 trans = glm::translate(glm::mat4(1.0f), model.position); // reposition model
			model.rotation = glm::rotate(model.rotation, glm::radians(0.4f), glm::vec3(0, 1, 0)); // rotate model on y axis
			glm::mat4 matrix = trans * model.rotation;
			glm::mat4 mvp = camera.genMVP(matrix);
			glm::mat3 mvi = glm::mat3(glm::transpose(glm::inverse(matrix)));
			shader.set3x3f(mvi_u, mvi);
			shader.set4x4f(model_u, matrix);
			shader.set4x4f(mvp_u, mvp);

			model.draw(shader);
		}



		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}
}


int main(int argc, char **argv)
{
	if (!glfwInit()) return -1;
    Window window = Window(800, 600, "Chirp");
	glfwMakeContextCurrent(window.window);

	glewInit();
	std::cout << glGetString(GL_RENDERER) << ", " << glGetString(GL_VERSION) << std::endl;

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	displayLoop(window);

	std::cout << "GL Error: " << glGetError();
	glfwTerminate();
	return 0;
}