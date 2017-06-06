#include<iostream>	
#include<sstream>
#include<memory>
#include<glew.h>
#include<glfw3.h>
#include<glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include<vector>
#include "glm/gtx/rotate_vector.hpp"
#include <map>

#include "Shader.hpp"
#include "Scene/Cube.hpp"
#include "Scene/Skybox.hpp"
#include "Scene/LightCube.hpp"
#include "Scene\TextQuad.hpp"
#include "Scene\MovingCube.hpp"
#include "Scene\Camera.hpp"
#include "Textures\TextureLoader.hpp"
#include "Scene\Model.hpp"

/* Freetype is used for the HUD -> to draw 2D characters to screen */
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace supernova;
using namespace supernova::scene;

void init(GLFWwindow* window);
void update(float time_delta, int pressed);
void draw();
void cleanup();
void initTextures();
void prepareFreeTypeCharacters();
void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, std::map<GLchar, Character> chars);

//make non global later!
// Shaders
std::unique_ptr<Shader> shader;
std::unique_ptr<Shader> skyboxShader;
std::unique_ptr<Shader> lightCubeShader;
std::unique_ptr<Shader> hudShader;
std::unique_ptr<Shader> textureShader;

// Game objects
std::unique_ptr<Skybox> skybox;
std::unique_ptr<MovingCube> startCube;
std::unique_ptr<MovingCube> midCube;
std::unique_ptr<MovingCube> finishCube;
std::unique_ptr<LightCube> lightCube;
std::unique_ptr<TextQuad> textQuad;
std::unique_ptr<Model> spaceship;
std::unique_ptr<Model> sun;

// Camera
std::unique_ptr<Camera> camera;

// Textuer loader
std::unique_ptr<TextureLoader> textureLoader = std::make_unique<TextureLoader>();

// Textures
GLuint cubeMapTexture;

// FreeType
FT_Library ft;
FT_Face face;
std::map<GLchar, Character> characters;

//camera
glm::vec3 cameraPos;
glm::vec3 cameraFront;
glm::vec3 cameraUp;
glm::mat4 view;

glm::mat4 projection;

int width = 1376;
int height = 768;
bool fullscreen = false;

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
		if (argc >= 4) {
			if ((std::stringstream(argv[3]) >> fullscreen).fail()) {
				std::cerr << "ERROR: Could not parse third command-line-argument as boolean." << std::endl;
				system("PAUSE");
				exit(EXIT_FAILURE);
			}
		}
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = nullptr;
	if (fullscreen) {
		monitor = glfwGetPrimaryMonitor();

		int refreshRate = 60;
		glfwWindowHint(GLFW_REFRESH_RATE, refreshRate);
	}

	auto window = glfwCreateWindow(width, height, "Test Window", monitor, nullptr);

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

	initTextures();
	init(window);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//used to get the first position of the mouse!
	bool firstMouse = true;
	GLfloat lastX = width / 2, lastY = height / 2;
	GLfloat pitch = 0.0f, yaw = 90.0f;

	bool running = true;
	auto time = glfwGetTime();

	// Wireframe
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	while (running && !glfwWindowShouldClose(window)) {

		//clear frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//compute frame time delta
		auto time_new = glfwGetTime();
		auto time_delta = (float)(time_new - time);
		time = time_new;

		/* Camera position console output */
		//std::cout << "frametime:" << time_delta * 1000 << "ms =~" << 1.0 / time_delta << "fps" << std::endl;
		//std::cout << "Camera position: " << cameraPos.x << ", " << cameraPos.y << ", " << cameraPos.z << std::endl;

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
		else if (glfwGetKey(window, GLFW_KEY_LEFT)) {
			pressed = -2;
		}
		else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
			pressed = -3;
		}

		//update camera
		bool forward = false;
		bool backward = false;
		bool rollLeft = false;
		bool rollRight = false;
		if (glfwGetKey(window, GLFW_KEY_W)) {
			forward = true;
		}
		if (glfwGetKey(window, GLFW_KEY_S)) {
			backward = true;
		}
		if (glfwGetKey(window, GLFW_KEY_A)) {
			rollLeft = true;
		}
		if (glfwGetKey(window, GLFW_KEY_D)) {
			rollRight = true;
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

		camera->update(forward, backward, rollLeft, rollRight, xoffset, yoffset, time_delta);

		//generate view projection matrix
		view = camera->viewMatrix;

		//update game components
		update(time_delta, pressed);

		//draw game components
		draw();

		// Swap buffers
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
	/* Enable Blending for FreeType (HUD) */
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/* FreeType - HUD */
	if (FT_Init_FreeType(&ft)) {
		std::cout << "ERROR::FREETYPE: Could not init Freetype library!" << std::endl;
	}
	if (FT_New_Face(ft, "Fonts/calibri.ttf", 0, &face)) {
		std::cout << "ERROR::FREETYPE: Failed to load font!" << std::endl;
	}
	FT_Set_Pixel_Sizes(face, 0, 48);
	prepareFreeTypeCharacters();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glfwSetWindowTitle(window, "Supernova");

	glClearColor(0.35f, 0.36f, 0.43f, 0.3f);
	glViewport(0, 0, width, height);

	// camera
	cameraPos = glm::vec3(0.0f, 0.0f, -5.0f);
	cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
	cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	camera = std::make_unique<Camera>(cameraPos, cameraFront, cameraUp, glm::cross(cameraFront, cameraUp));
	view = camera->viewMatrix;

	/* Step 1: Create shaders */
	shader = std::make_unique<Shader>("Shader/basic.vert", "Shader/basic.frag");
	skyboxShader = std::make_unique<Shader>("Shader/skybox.vert", "Shader/skybox.frag");
	lightCubeShader = std::make_unique<Shader>("Shader/lightCube.vert", "Shader/lightCube.frag");
	hudShader = std::make_unique<Shader>("Shader/hud.vert", "Shader/hud.frag");
	textureShader = std::make_unique<Shader>("Shader/textureShader.vert", "Shader/textureShader.frag");// Shader for textured objects

	/* Step 2: Create scene objects and assign shaders */
	skybox = std::make_unique<Skybox>(glm::mat4(1.0f), skyboxShader.get(), cubeMapTexture);
	lightCube = std::make_unique<LightCube>(glm::mat4(1.0f), lightCubeShader.get());
	startCube = std::make_unique<MovingCube>(glm::mat4(1.0f), shader.get(), camera.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)), 5.0f);
	midCube = std::make_unique<MovingCube>(glm::mat4(1.0f), shader.get(), camera.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)), 15.0f);
	finishCube = std::make_unique<MovingCube>(glm::mat4(1.0f), shader.get(), camera.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)), 25.0f);
	//string spaceshipDir = "Models/nanosuit/nanosuit.obj";
	string spaceshipDir = "Models/spaceship/spaceship.obj";

	spaceship = std::make_unique<Model>(glm::mat4(1.0f), spaceshipDir);
	sun = std::make_unique<Model>(glm::mat4(1.0f), "Models/icosphere/icosphere.obj");
	textQuad = std::make_unique<TextQuad>(glm::mat4(1.0f), hudShader.get());

	/* Step 3: Use those shaders */
	shader->useShader();
	lightCubeShader->useShader();
	textureShader->useShader();
	skyboxShader->useShader();
	hudShader->useShader();

	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);

	/* glm::perspective takes (fov, aspect, nearPlane, farPlane) */
	projection = glm::perspective(30.0f, width / (float)height, 0.1f, 50.0f);

	}

void update(float time_delta, int pressed) {
	skybox->update(time_delta, pressed);

	lightCube->update(time_delta, pressed);
	startCube->update(time_delta, pressed);
	midCube->update(time_delta, pressed);
	finishCube->update(time_delta, pressed);
	spaceship->update(time_delta, pressed);

	textQuad->update(time_delta, pressed);
}

void draw() {
	//generate view projection matrix
	auto view_projection = projection * view;

	/*-------------------- Textured objects --------------------- */

	// Spaceship
	/*textureShader->useShader();
	auto view_projection_location_spaceship = glGetUniformLocation(textureShader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_spaceship, 1, GL_FALSE, glm::value_ptr(view_projection));
	auto& model_spaceship = spaceship->modelMatrix;
	auto model_location_spaceship = glGetUniformLocation(textureShader->programHandle, "model");
	glUniformMatrix4fv(model_location_spaceship, 1, GL_FALSE, glm::value_ptr(model_spaceship));
	spaceship->draw(textureShader.get());*/

	/* Light Cube */
	lightCubeShader->useShader();
	auto view_projection_location_light_cube = glGetUniformLocation(lightCubeShader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_light_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

	// make light cube a little bit smaller and put it schräg above other cube
	auto& model_light_cube = glm::scale(lightCube->modelMatrix, glm::vec3(0.3f));
	model_light_cube = glm::translate(model_light_cube, glm::vec3(2.5f, -5.5f, 1.5f));
	auto model_location_light_cube = glGetUniformLocation(lightCubeShader->programHandle, "model");
	glUniformMatrix4fv(model_location_light_cube, 1, GL_FALSE, glm::value_ptr(model_light_cube));
	lightCube->draw();

	// Sun
	shader->useShader();
	auto view_projection_location_sun = glGetUniformLocation(shader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_sun, 1, GL_FALSE, glm::value_ptr(view_projection));
	auto& model_sun = sun->modelMatrix;
	auto model_location_sun = glGetUniformLocation(shader->programHandle, "model");
	glUniformMatrix4fv(model_location_sun, 1, GL_FALSE, glm::value_ptr(model_sun));
	sun->draw(shader.get());

	/* Cube */
	shader->useShader();
	auto view_projection_location_cube = glGetUniformLocation(shader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

	// view(camera) info
	GLint cameraPosLoc = glGetUniformLocation(shader->programHandle, "cameraPos");
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	// Light position and properties (currently only one lightsource)
	glm::vec3 lightPositionInWorldSpace = glm::vec3(model_light_cube[3].x, model_light_cube[3].y,
		model_light_cube[3].z);
	GLint lightPosLoc = glGetUniformLocation(shader->programHandle, "light.position");
	GLint lightAmbientLoc = glGetUniformLocation(shader->programHandle, "light.ambient");
	GLint lightDiffuseLoc = glGetUniformLocation(shader->programHandle, "light.diffuse");
	GLint lightSpecularLoc = glGetUniformLocation(shader->programHandle, "light.specular");

	glUniform3f(lightPosLoc, lightPositionInWorldSpace.x, lightPositionInWorldSpace.y, lightPositionInWorldSpace.z);
	glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

	/* Start Cube */
	view_projection_location_cube = glGetUniformLocation(shader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

	auto& model_moving_cube = startCube->modelMatrix;
	auto model_location_moving_cube = glGetUniformLocation(shader->programHandle, "model");
	glUniformMatrix4fv(model_location_moving_cube, 1, GL_FALSE, glm::value_ptr(model_moving_cube));

	// view(camera) info
	cameraPosLoc = glGetUniformLocation(shader->programHandle, "cameraPos");
	glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	// Light position and properties (currently only one lightsource)
	lightPosLoc = glGetUniformLocation(shader->programHandle, "light.position");
	lightAmbientLoc = glGetUniformLocation(shader->programHandle, "light.ambient");
	lightDiffuseLoc = glGetUniformLocation(shader->programHandle, "light.diffuse");
	lightSpecularLoc = glGetUniformLocation(shader->programHandle, "light.specular");

	glUniform3f(lightPosLoc, lightPositionInWorldSpace.x, lightPositionInWorldSpace.y, lightPositionInWorldSpace.z);
	glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
	startCube->draw();

	// Mid Cube
	auto& model_mid_cube = midCube->modelMatrix;
	auto model_location_mid_cube = glGetUniformLocation(shader->programHandle, "model");
	glUniformMatrix4fv(model_location_moving_cube, 1, GL_FALSE, glm::value_ptr(model_mid_cube));
	midCube->draw();

	// Finish Cube
	auto& model_finish_cube = finishCube->modelMatrix;
	auto model_location_finish_cube = glGetUniformLocation(shader->programHandle, "model");
	glUniformMatrix4fv(model_location_moving_cube, 1, GL_FALSE, glm::value_ptr(model_finish_cube));
	finishCube->draw();


	/* Skybox - ALWAYS DRAW LAST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11elf
	* Only 2d objects are allowed to be drawn after skybox
	/* Change depth function so depth test passes when values are equal to depth buffers content */
	glDepthFunc(GL_LEQUAL);
	glDepthMask(GL_FALSE);
	skyboxShader->useShader();
	/* Remove translation component from matrix for skybox */
	view = glm::mat4(glm::mat3(view));

	auto& skyboxModel = skybox->modelMatrix;
	auto skyboxModel_location = glGetUniformLocation(skyboxShader->programHandle, "model");
	glUniformMatrix4fv(skyboxModel_location, 1, GL_FALSE, glm::value_ptr(skyboxModel));

	view_projection = projection * view;
	glUniformMatrix4fv(glGetUniformLocation(skyboxShader->programHandle, "proj"),
		1, GL_FALSE, glm::value_ptr(view_projection));

	skybox->draw();

	/* Reset depth function */
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	/* HUD */
	glm::mat4 hudProjectionMat = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	hudShader->useShader();
	glUniformMatrix4fv(glGetUniformLocation(hudShader->programHandle, "projection"), 1, GL_FALSE, glm::value_ptr(hudProjectionMat));
	/* If user is outside boundary(40x40x70) display warning */
	if (camera.get()->getPosition().x < -20 
		|| camera.get()->getPosition().x > 20 
		|| camera.get()->getPosition().y < -20
		|| camera.get()->getPosition().y > 20
		|| camera.get()->getPosition().z < -10
		|| camera.get()->getPosition().z > 60) {
			renderText("Danger Zone.", 550.0f, 380.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
	}
	if (camera.get()->getPosition().z > 25) {
		renderText("You win!", 550.0f, 380.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
	}

}

void cleanup() {
	shader.reset(nullptr);
	/* Light Cube */
	lightCube.reset(nullptr);
	lightCubeShader.reset(nullptr);
	/* Moving Cube */
	startCube.reset(nullptr);
	midCube.reset(nullptr);
	finishCube.reset(nullptr);
	/* Skybox */
	skybox.reset(nullptr);
	skyboxShader.reset(nullptr);
	/* HUD */
	textQuad.reset(nullptr);
	hudShader.reset(nullptr);
	/* Camera */
	camera.reset(nullptr);
	/* Spaceship */
	textureShader.reset(nullptr);
	spaceship.reset(nullptr);
}

void initTextures() {
	FreeImage_Initialise(true);
	// Cubemap (Skybox)
	std::vector<const GLchar*> faces;
	faces.push_back("Textures/Skybox/purplenebula_rt.png");
	faces.push_back("Textures/Skybox/purplenebula_lf.png");
	faces.push_back("Textures/Skybox/purplenebula_up.png");
	faces.push_back("Textures/Skybox/purplenebula_dn.png");
	faces.push_back("Textures/Skybox/purplenebula_bk.png");
	faces.push_back("Textures/Skybox/purplenebula_ft.png");
	cubeMapTexture = textureLoader.get()->loadCubemap(faces);
}

/* Called in init() - Loads a font and stores its rendered characters in a map */
void prepareFreeTypeCharacters() {
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	/* For now only first 128 ASCII characters */
	for (GLubyte c = 0; c < 128; c++) {
		// Get char glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "ERROR::FREETYPE: Failed to load Glyph!" << std::endl;
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture params
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// Store data in our struct
		Character character = {
			texture, glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		characters.insert(std::pair<GLchar, Character>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// Clear resources
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

/* Renders text to HUD */
void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, std::map<GLchar, Character> chars) {
	hudShader->useShader();
	textQuad->draw(text, x, y, scale, color, chars);
}