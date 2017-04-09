#include<iostream>	
#include<sstream>
#include<memory>
#include<glew.h>
#include<glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"

#include "Shader.hpp"
#include "Scene/Cube.hpp"
using namespace supernova;
using namespace supernova::scene;

void init(GLFWwindow* window);
void update(float time_delta, int pressed);
void draw();
void cleanup();

//make non global later!
std::unique_ptr<Shader> shader;
std::unique_ptr<Cube> cube;

//camera
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
glm::mat4 view;

glm::mat4 projection;

int width = 1280;
int height = 720;

void main(int argc, char** argv) {

	if (!glfwInit()) {
		std::cerr << "ERROR: Could not init glfw" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	if (argc >= 3) {
		if ((std::stringstream(argv[1]) >> width).fail()) {
			std::cerr << "ERROR: Could not parse first command-line-argument as integer." << std::endl;
			system("PAUSE");
			exit(EXIT_FAILURE);
		}
		if ((std::stringstream(argv[2]) >> height).fail()) {
			std::cerr << "ERROR: Could not parse second command-line-argument as integer." << std::endl;
			system("PAUSE");
			exit(EXIT_FAILURE);
		}
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	auto window = glfwCreateWindow(width, height, "Test Window", nullptr, nullptr);

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

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//used to get the first position of the mouse!
	bool firstMouse = true;
	GLfloat lastX = width / 2, lastY = height / 2;
	GLfloat pitch = 0.0f, yaw = 90.0f;

	bool running = true;
	auto time = glfwGetTime();

	while (running && !glfwWindowShouldClose(window)) {

		//clear frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//compute frame time delta
		auto time_new = glfwGetTime();
		auto time_delta = (float)(time_new - time);
		time = time_new;

		std::cout << "frametime:" << time_delta * 1000 << "ms =~" << 1.0 / time_delta << "fps" << std::endl;

		//react to user input (maybe extract key and mouse input to seperate methods!)
		glfwPollEvents();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			running = false;
			glfwSetWindowShouldClose(window, true);
		}

		int pressed = 0;
		if (glfwGetKey(window, GLFW_KEY_UP)) {
			pressed = 1;
		}
		else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
			pressed = -1;
		}

		//update camera
		float cameraSpeed = 5.0f * time_delta;
		if (glfwGetKey(window, GLFW_KEY_W)) {
			cameraPos += cameraSpeed * cameraFront;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			cameraPos -= cameraSpeed * cameraFront;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			cameraPos += cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			cameraPos -= cameraSpeed * glm::normalize(glm::cross(cameraFront, cameraUp));
		}


		double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		if (firstMouse) {
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		//change as you want! Maybe make changable in settings
		GLfloat sensitivity = 0.2f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;

		yaw -= xoffset;
		pitch -= yoffset;

		glm::vec3 front;
		front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		front.y = sin(glm::radians(pitch));
		front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(front);

		//generate view projection matrix
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		auto view_projection = projection * view;
		auto view_projection_location = glGetUniformLocation(shader->programHandle, "proj");
		glUniformMatrix4fv(view_projection_location, 1, GL_FALSE, glm::value_ptr(view_projection));

		//update game components
		update(time_delta, pressed);

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
	glEnable(GL_DEPTH_TEST);
	glfwSetWindowTitle(window, "Supernova");

	glClearColor(0.35f, 0.36f, 0.43f, 0.3f);
	glViewport(0, 0, width, height);

	shader = std::make_unique<Shader>("Shader/basic.vert", "Shader/basic.frag");
	cube = std::make_unique<Cube>(glm::mat4(1.0f), shader.get());

	shader->useShader();

	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);

	projection = glm::perspective(30.0f, width / (float)height, 0.1f, 20.0f);

	//camera
	cameraPos = glm::vec3(0.0f, 0.0f, -2.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
}

void update(float time_delta, int pressed) {
	cube->update(time_delta, pressed);
}

void draw() {
	auto& model = cube->modelMatrix;

	auto model_location = glGetUniformLocation(shader->programHandle, "model");

	glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));

	shader->useShader();
	cube->draw();
}

void cleanup() {
	cube.reset(nullptr);
	shader.reset(nullptr);
}