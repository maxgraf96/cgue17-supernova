#include<iostream>	
#include<glew.h>
#include<glfw3.h>

#include "Shader.hpp"
using namespace supernova;

void init(GLFWwindow* window);
void update(float time_delta);
void draw();
void cleanup();

const int width = 1280;
const int height = 720;

void main() {

	if (!glfwInit()) {
		std::cerr << "ERROR: Could not init glfw" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr); //TODO: chagnge title

	if (!window) {
		std::cerr << "ERROR: Could not open window" << std::endl;
		glfwTerminate;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		std::cerr << "ERROR: Could not initialize glew." << std::endl;
		glfwTerminate;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	init(window);

	bool running = true;
	auto time = glfwGetTime();

	while (running && !glfwWindowShouldClose(window)) {

		//cleare frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//compute frame time delta
		auto time_new = glfwGetTime();
		auto time_delta = (float)(time_new - time);
		time = time_new;

		std::cout << "frametime:" << time_delta * 1000 << "ms =~" << 1.0 / time_delta << "fps" << std::endl;

		//react to user input
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			running = false;
			glfwSetWindowShouldClose(window, true);
		}

		//update game components
		update(time_delta);

		//draw game components
		draw();
		glfwSwapBuffers(window);

		//catch errors
		GLenum error = glGetError();

		if (error != GL_NO_ERROR) {
			switch (error) {
			case GL_INVALID_ENUM:
				std::cerr << "GL: enum argument out of range." << std::endl;
				break;
			case GL_INVALID_VALUE:
				std::cerr << "GL: Numeric argument out of range." << std::endl;
				break;
			case GL_INVALID_OPERATION:
				std::cerr << "GL: Operation illegal in current state." << std::endl;
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				std::cerr << "GL: Framebuffer object is not complete." << std::endl;
				break;
			case GL_OUT_OF_MEMORY:
				std::cerr << "GL: Not enough memory left to execute command." << std::endl;
				break;
			default:
				std::cerr << "GL: Unknown error." << std::endl;
			}
		}
	}

	//cleanup!
	cleanup();
	glfwTerminate();

}

void init(GLFWwindow* window) {
	glfwSetWindowTitle(window, "Supernova");

	glClearColor(0.35f, 0.36f, 0.43f, 0.3f);
	glViewport(0, 0, width, height);
}

void update(float time_delta) {

}

void draw() {

}

void cleanup() {

}