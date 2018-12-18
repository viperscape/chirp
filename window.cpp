#include "window.h"



Window::Window(int x, int y, const char* title)
{
	GLFWwindow* window = glfwCreateWindow(x, y, title, NULL, NULL);
	glfwSetWindowPos(window, 100, 100);
	this->window = window;
}


Window::~Window()
{
	glfwDestroyWindow(this->window);
}

void Window::resize() 
{
	if (!this->window) return;

	GLint w, h;
	glfwGetWindowSize(this->window, &w, &h);
	if ((w>0) && (h>0))
		glViewport(0, 0, w, h);
}

int Window::close()
{
	return glfwWindowShouldClose(this->window);
}
