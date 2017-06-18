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
#include <physx-3.4\PxPhysicsAPI.h>
#include <physx-3.4\extensions\PxSimpleFactory.h>

#include "Shader.hpp"
#include "Scene/Cube.hpp"
#include "Scene/Skybox.hpp"
#include "Scene/LightCube.hpp"
#include "Scene\TextQuad.hpp"
#include "Scene\MovingCube.hpp"
#include "Scene\Camera.hpp"
#include "Textures\TextureLoader.hpp"
#include "Scene\Model.hpp"
#include "Shader.hpp"
#include "Scene\PhysXCube.hpp"
#include "CollisionDetector.hpp"
//#include "Particles\ParticleTF.hpp"
#include "Particles\ExtPTF.hpp"
#include "Lights\Light.hpp"

/* Freetype is used for the HUD -> to draw 2D characters to screen */
#include <ft2build.h>
#include FT_FREETYPE_H

using namespace supernova;
using namespace supernova::scene;
using namespace physx;

void init(GLFWwindow* window);
void update(float time_delta, int pressed);
void draw();
void cleanup();
void initTextures();
void prepareFreeTypeCharacters();
void renderText(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, std::map<GLchar, Character> chars);
void initializePhysX();
void shutdownPhysX();
bool stepPhysX(float dt);

//make non global later!
// Shaders
std::unique_ptr<Shader> basicShader;
std::unique_ptr<Shader> skyboxShader;
std::unique_ptr<Shader> lightCubeShader;
std::unique_ptr<Shader> hudShader;
std::unique_ptr<Shader> textureShader;
std::unique_ptr<Shader> screenShader;
std::unique_ptr<Shader> blurShader;
std::unique_ptr<Shader> lensFlareShader;
std::unique_ptr<Shader> motionBlurShader;
std::unique_ptr<Shader> sunShader;

// Game objects
std::unique_ptr<Skybox> skybox;
std::unique_ptr<MovingCube> startCube;
std::unique_ptr<MovingCube> midCube;
std::unique_ptr<MovingCube> finishCube;
std::unique_ptr<TextQuad> textQuad;
std::unique_ptr<Spaceship> spaceship;
std::unique_ptr<PhysXCube> physXCube;
std::unique_ptr<LightCube> lightCube;
std::unique_ptr<Sun> sun;

// Camera
std::unique_ptr<Camera> camera;

// Textuer loader
std::unique_ptr<TextureLoader> textureLoader = std::make_unique<TextureLoader>();

// Textures
GLuint cubeMapTexture;
GLuint particle;
GLuint lensflaresColor;

//Framebuffer
GLuint framebuffer;
GLuint texColorBuffer[4];
GLuint depthBuffer;

//Pingpong Framebuffer
GLuint pingpongFBO[2];
GLuint pingpongBuffer[2];

//LensFlares Framebuffer
GLuint lensFlaresFBO;
GLuint lensFlaresBuffer;

// Motion blur framebuffer and helper objects
GLuint motionBlurFBO;
GLuint motionBlurColorOut;
// Previous MVP matrix for comparing pixel positions
glm::mat4 previousVP; // VP Matrix of previous frame

// FreeType
FT_Library ft;
FT_Face face;
std::map<GLchar, Character> characters;

//PhysX
PxFoundation* gFoundation = NULL;
PxPhysics* gPhysics = NULL;
PxScene* gScene = NULL;
PxDefaultCpuDispatcher* gDispatcher = NULL;
PxMaterial* gMaterial = NULL;

PxDefaultErrorCallback gErrorCallback;
PxDefaultAllocator gAllocator;

float gAccumulator = 0.0f;
float gStepSize = 1.0f / 60.0f;

/* one vector that contains all point light sources. This vector is then passed to all shaders of objects that are to be lit. lit. */
std::vector<PointLight> pointLights;
std::vector<DirectionalLight> dirLights;

//camera
glm::mat4 view;
glm::mat4 projection;

int width = 1376;
int height = 768;
bool fullscreen = false;

float time_delta = 0;

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

	//maybe put in init function!
	float quadVertices[] = {
		-1.0f,  1.0f,  0.0f,  1.0f,
		-1.0f, -1.0f,  0.0f,  0.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,

		-1.0f,  1.0f,  0.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,  0.0f,
		 1.0f,  1.0f,  1.0f,  1.0f
	};

	GLuint quadVAO, quadVBO;
	glGenVertexArrays(1, &quadVAO);
	glGenBuffers(1, &quadVBO);
	glBindVertexArray(quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

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
		time_delta = (float)(time_new - time);
		time = time_new;

		/* Camera position console output */
		//std::cout << "frametime:" << time_delta * 1000 << "ms =~" << 1.0 / time_delta << "fps" << std::endl;

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

		//update game components
		update(time_delta, pressed);

		//update spaceship
		spaceship->update(forward, backward, rollLeft, rollRight, xoffset, yoffset, time_delta);

		//update camera
		glm::vec3  cameraFront = spaceship->front;
		glm::vec3  cameraUp = spaceship->up * -1.0f;
		glm::vec3  cameraRight = spaceship->right;
		glm::vec3 cameraPos = spaceship->getPosition();
		cameraPos = cameraPos - 12.0f * cameraFront;
		cameraPos = cameraPos - 2.0f * cameraUp;

		camera->update(cameraPos, cameraFront, cameraUp, cameraRight);

		//generate view matrix
		view = camera->viewMatrix;

		//simulate PhysX (do somewhere else?)
		stepPhysX(time_delta);

		/*****START RENDER*****/
		//based on the framebuffers tutorial from "Learn OpenGL" (https://learnopengl.com/#!Advanced-OpenGL/Framebuffers)

		//first pass, draw to framebuffer
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST); //could cause trouble
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw();

		/******POST PROCESSING******/
		// Motion blur
		glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO);
		motionBlurShader->useShader();

		// Current model->view matrix 
		glm::mat4 inverseCurrentVP = glm::inverse(camera->viewMatrix * projection);
		motionBlurShader->setUniform("inverseVP", inverseCurrentVP);
		motionBlurShader->setUniform("previousVP", previousVP);
		motionBlurShader->setUniform("fps", 1.0f / time_delta);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer[0]);
		motionBlurShader->setUniform("texColor", 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthBuffer);
		motionBlurShader->setUniform("texDepth", 1);

		//render quad
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		//Blur bright colors using pingpong framebuffers
		bool horizontal = true;
		bool first_iter = true;
		int amount = 10;
		blurShader->useShader();

		for (GLuint i = 0; i < amount; i++) {
			glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[horizontal]);
			blurShader->setUniform("horizontal", horizontal);
			glBindTexture(GL_TEXTURE_2D, first_iter ? texColorBuffer[1] : pingpongBuffer[!horizontal]);

			//render quad
			glBindVertexArray(quadVAO);
			glDrawArrays(GL_TRIANGLES, 0, 6);
			glBindVertexArray(0);

			horizontal = !horizontal;
			if (first_iter) {
				first_iter = false;
			}
		}

		//Lens Flares
		lensFlareShader->useShader();
		int ghosts = 4; //number of ghost samples
		float ghostDispersal = 0.5;
		float haloWidth = 0.5;
		float distortion = 3.0;

		glBindFramebuffer(GL_FRAMEBUFFER, lensFlaresFBO);
		lensFlareShader->setUniform("ghosts", ghosts);
		lensFlareShader->setUniform("ghostDispersal", ghostDispersal);
		lensFlareShader->setUniform("haloWidth", haloWidth);
		lensFlareShader->setUniform("distortion", distortion);
		
		glBindTexture(GL_TEXTURE_2D, texColorBuffer[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, lensflaresColor);
		//render quad
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		/******RENDER TO SCREEN******/
		glBindFramebuffer(GL_FRAMEBUFFER, 0); //using default to render to screen
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//maybe put into method (e.g. renderToScreen)
		screenShader->useShader();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texColorBuffer[0]);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[!horizontal]);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, lensFlaresBuffer);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, motionBlurColorOut);
		
		glBindVertexArray(quadVAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		
		/*****END OF RENDER*****/
		mat4 test = lightCube->modelMatrix;
		previousVP = camera->viewMatrix * projection;// previous vp = current vp

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
	//initialize PhysX
	initializePhysX();

	
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

	//glClearColor(0.35f, 0.36f, 0.43f, 0.3f);
	glViewport(0, 0, width, height);

	/* Step 1: Create shaders */
	// Objects
	textureShader = std::make_unique<Shader>("Shader/textureShader.vert", "Shader/textureShader.frag", true);
	basicShader = std::make_unique<Shader>("Shader/basic.vert", "Shader/basic.frag", true);
	skyboxShader = std::make_unique<Shader>("Shader/skybox.vert", "Shader/skybox.frag", true);
	lightCubeShader = std::make_unique<Shader>("Shader/lightCube.vert", "Shader/lightCube.frag", true);
	sunShader = std::make_unique<Shader>("Shader/sun.vert", "Shader/sun.frag", true);

	// Effects
	hudShader = std::make_unique<Shader>("Shader/hud.vert", "Shader/hud.frag", true);
	screenShader = std::make_unique<Shader>("Shader/screenShader.vert", "Shader/screenShader.frag", true);
	blurShader = std::make_unique<Shader>("Shader/blur.vert", "Shader/blur.frag", true);
	lensFlareShader = std::make_unique<Shader>("Shader/lensflares.vert", "Shader/lensflares.frag", true);
	motionBlurShader = std::make_unique<Shader>("Shader/motionBlur.vert", "Shader/motionBlur.frag", true);

	// Link shaders
	textureShader->link();
	basicShader->link();
	skyboxShader->link();
	lightCubeShader->link();
	sunShader->link();

	hudShader->link();
	screenShader->link();
	blurShader->link();
	lensFlareShader->link();
	motionBlurShader->link();

	//initiate textures
	screenShader->useShader();
	screenShader->setUniform("scene", 0);
	screenShader->setUniform("bloom", 1);
	screenShader->setUniform("lensflares", 2);
	screenShader->setUniform("motionBlur", 3);

	lensFlareShader->useShader();
	lensFlareShader->setUniform("input", 0);
	lensFlareShader->setUniform("lensflaresColor", 1);

	motionBlurShader->useShader();
	motionBlurShader->setUniform("texColor", 0);
	motionBlurShader->setUniform("texDepth", 1);

	/* Step 2: Create scene objects and assign shaders */
	// camera
	spaceship = std::make_unique<Spaceship>(glm::mat4(1.0f), "Models/spaceship/spaceship.obj");
	spaceship->translate(vec3(0.0f, 0.0f, -50.0f));

	glm::vec3  cameraFront = spaceship->front;
	glm::vec3  cameraUp = spaceship->up * -1.0f;
	glm::vec3  cameraRight = spaceship->right;
	glm::vec3 cameraPos = spaceship->getPosition();
	cameraPos = cameraPos - 12.0f * cameraFront;
	cameraPos = cameraPos - 2.0f * cameraUp;

	camera = std::make_unique<Camera>(cameraPos, cameraFront, cameraUp, cameraRight);
	view = camera->viewMatrix;

	skybox = std::make_unique<Skybox>(glm::mat4(1.0f), skyboxShader.get(), cubeMapTexture);
	startCube = std::make_unique<MovingCube>(glm::mat4(1.0f), basicShader.get(), camera.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)), 5.0f);
	midCube = std::make_unique<MovingCube>(glm::mat4(1.0f), basicShader.get(), camera.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)), 15.0f);
	finishCube = std::make_unique<MovingCube>(glm::mat4(1.0f), basicShader.get(), camera.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)), 25.0f);

	/* Create lights */
	sun = std::make_unique<Sun>(glm::mat4(1.0f), "Models/newsun/newsun.obj");
	vec3 sunLightColor = vec3(1.0f);
	vec3 weaker = vec3(0.5f, 0.5f, 0.5f);

	// Reasonably placed directional lights to simulate the sun
	dirLights.push_back(*std::make_unique<DirectionalLight>(sunLightColor, sunLightColor, sunLightColor, vec3(0, 1, 0)).get());
	dirLights.push_back(*std::make_unique<DirectionalLight>(sunLightColor, sunLightColor, sunLightColor, vec3(0, -1, 0)).get());

	// Two from directly behind
	dirLights.push_back(*std::make_unique<DirectionalLight>(sunLightColor, sunLightColor, sunLightColor, vec3(0, 0, 1)).get());
	dirLights.push_back(*std::make_unique<DirectionalLight>(sunLightColor, sunLightColor, sunLightColor, vec3(0, 0, 1)).get());

	dirLights.push_back(*std::make_unique<DirectionalLight>(sunLightColor, sunLightColor, sunLightColor, vec3(0.2f, 1, 1)).get());
	dirLights.push_back(*std::make_unique<DirectionalLight>(sunLightColor, sunLightColor, sunLightColor, vec3(-0.2f, -1, 1)).get());

	for (DirectionalLight &l : dirLights)
		l.setInitialized(true);


	vec3 lightCubeColor = vec3(0, 0, 0);
	lightCube = std::make_unique<LightCube>(glm::mat4(1.0f), lightCubeShader.get());
	PointLight cubeLight = PointLight(lightCubeColor, lightCubeColor, lightCubeColor, lightCube->getPosition(),
		1.0f, 0.9f, 0.2f);
	cubeLight.setInitialized(true);
	pointLights.push_back(cubeLight);

	
	
	/******SETUP FRAMEBUFFER******/
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	//generate two textures (colorbuffers) and attach them to framebuffer
	glGenTextures(4, texColorBuffer);
	for (GLuint i = 0; i < 3; i++) {
		glBindTexture(GL_TEXTURE_2D, texColorBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, texColorBuffer[i], 0);
	}

	// Add motion blur depth buffer -> this one writes to a texture instead of being a renderbuffer
	glGenTextures(1, &depthBuffer);
	glBindTexture(GL_TEXTURE_2D, depthBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthBuffer, 0);

	//tell OpenGL this framebuffer will use multiple color attachments
	GLuint attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
	glDrawBuffers(4, attachments);

	//check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/******SETUP PINGPONG FRAMEBUFFERS******/
	//based on the Bloom tutorial from "Learn OpenGL" (https://learnopengl.com/#!Advanced-Lighting/Bloom)
	glGenFramebuffers(2, pingpongFBO);
	glGenTextures(2, pingpongBuffer);
	for (GLuint i = 0; i < 2; i++) {
		glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, pingpongBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongBuffer[i], 0);

		//check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/******SETUP LENSFLARES FRAMEBUFFER******/
	//based on the lensflares tutorial from John Chapman (http://john-chapman-graphics.blogspot.co.at/2013/02/pseudo-lens-flare.html)
	glGenFramebuffers(1, &lensFlaresFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, lensFlaresFBO);

	glGenTextures(1, &lensFlaresBuffer);
	glBindTexture(GL_TEXTURE_2D, lensFlaresBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, lensFlaresBuffer, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	/******SETUP MOTION BLUR FRAMEBUFFER******/
	glGenFramebuffers(1, &motionBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, motionBlurFBO);

	glGenTextures(1, &motionBlurColorOut);
	glBindTexture(GL_TEXTURE_2D, motionBlurColorOut);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, motionBlurColorOut, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//Create PhysXcube:
	PxReal density = 1.0f;
	PxTransform transform(PxVec3(0.0f, 10.0f, 0.0f), PxQuat(PxIDENTITY::PxIdentity));
	PxVec3 dimensions(0.5f, 0.5f, 0.5f);
	PxBoxGeometry geometry(dimensions);

	PxRigidDynamic *actor = PxCreateDynamic(*gPhysics, transform, geometry, *gMaterial, density);
	actor->setAngularDamping(0.75f);
	actor->setLinearVelocity(PxVec3(0, 0, 0));
	if (!actor) {
		cerr << "create actor failed" << endl;
	}
	gScene->addActor(*actor);
	PxRigidActor* box = actor;

	physXCube = std::make_unique<PhysXCube>(glm::mat4(1.0f), box, basicShader.get(), new Metal(vec3(0.905f, 0.298f, 0.235f)));
	
	// HUD
	textQuad = std::make_unique<TextQuad>(glm::mat4(1.0f), hudShader.get());

	// Particle system(s)
	spaceship->particleSystem.InitalizeParticleSystem();
	spaceship->particleSystem.SetGeneratorProperties(
		glm::vec3(0.0f, 0.0f, 0.0f), // Where the particles are generated
		glm::vec3(-1, -1, 0), // Minimal velocity
		glm::vec3(5, 5, 5), // Maximal velocity
		glm::vec3(0, 0, 0), // Gravity force applied to particles
		glm::vec3(0.92f, 0.08f, 0.08f), // Color
		0.5f, // Minimum lifetime in seconds
		1.5f, // Maximum lifetime in seconds
		0.3f, // Rendered size
		0.05f, // Spawn every x seconds
		10// And spawn 30 particles);
		);

	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);

	/* glm::perspective takes (fov, aspect, nearPlane, farPlane) */
	projection = glm::perspective(30.0f, (float )width / (float)height, 0.1f, 2000.0f);

	}

void update(float time_delta, int pressed) {
	skybox->update(time_delta, pressed);

	lightCube->update(time_delta, pressed);
	startCube->update(time_delta, pressed);
	midCube->update(time_delta, pressed);
	finishCube->update(time_delta, pressed);

	textQuad->update(time_delta, pressed);
	sun->update(time_delta, pressed);

}

void draw() {
	//generate view projection matrix
	auto view_projection = projection * view;

	/*-------------------- Get light sources --------------------- */
	/* Light Cube */
	lightCubeShader->useShader();
	auto view_projection_location_light_cube = glGetUniformLocation(lightCubeShader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_light_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

	// make light cube a little bit smaller and put it schräg above other cube
	auto& model_light_cube = glm::scale(lightCube->modelMatrix, glm::vec3(3));
	model_light_cube = glm::translate(model_light_cube, glm::vec3(2.5f, -5.5f, -30));
	auto model_location_light_cube = glGetUniformLocation(lightCubeShader->programHandle, "model");
	glUniformMatrix4fv(model_location_light_cube, 1, GL_FALSE, glm::value_ptr(model_light_cube));
	lightCube->draw();

	// view(camera) info
	GLint cameraPosLoc;

	// Convert light vectors to arrays to pass them to shaders
	PointLight point_lights_array[20];
	DirectionalLight dir_lights_array[12];
	std::copy(pointLights.begin(), pointLights.end(), point_lights_array);
	std::copy(dirLights.begin(), dirLights.end(), dir_lights_array);

	/*-------------------- Textured objects --------------------- */
	// Spaceship
	textureShader->useShader();
	auto view_projection_location_spaceship = glGetUniformLocation(textureShader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_spaceship, 1, GL_FALSE, glm::value_ptr(view_projection));
	auto& model_spaceship = spaceship->modelMatrix;
	auto model_location_spaceship = glGetUniformLocation(textureShader->programHandle, "model");
	glUniformMatrix4fv(model_location_spaceship, 1, GL_FALSE, glm::value_ptr(model_spaceship));
	
	// SET ALL LIGHT SOURCES WITH THIS LIMITED EDITION ONE LINER
	textureShader->setLightSources(dir_lights_array, point_lights_array, camera.get());

	spaceship->draw(textureShader.get());

	// Sun
	sunShader->useShader();
	auto view_projection_location_sun = glGetUniformLocation(sunShader->programHandle, "proj");
	glUniformMatrix4fv(view_projection_location_sun, 1, GL_FALSE, glm::value_ptr(view_projection));
	auto& model_sun = sun->modelMatrix;
	auto model_location_sun = glGetUniformLocation(sunShader->programHandle, "model");
	glUniformMatrix4fv(model_location_sun, 1, GL_FALSE, glm::value_ptr(model_sun));
	sun->draw(sunShader.get());

	///* Cube */
	//shader->useShader();
	//auto view_projection_location_cube = glGetUniformLocation(shader->programHandle, "proj");
	//glUniformMatrix4fv(view_projection_location_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

	//lightPosLoc = glGetUniformLocation(shader->programHandle, "light.position");
	//lightAmbientLoc = glGetUniformLocation(shader->programHandle, "light.ambient");
	//lightDiffuseLoc = glGetUniformLocation(shader->programHandle, "light.diffuse");
	//lightSpecularLoc = glGetUniformLocation(shader->programHandle, "light.specular");

	//glUniform3f(lightPosLoc, lightPositionInWorldSpace.x, lightPositionInWorldSpace.y, lightPositionInWorldSpace.z);
	//glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	//glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	//glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);

	///* Start Cube */
	//view_projection_location_cube = glGetUniformLocation(shader->programHandle, "proj");
	//glUniformMatrix4fv(view_projection_location_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

	//auto& model_moving_cube = startCube->modelMatrix;
	//auto model_location_moving_cube = glGetUniformLocation(shader->programHandle, "model");
	//glUniformMatrix4fv(model_location_moving_cube, 1, GL_FALSE, glm::value_ptr(model_moving_cube));

	//// view(camera) info
	//cameraPosLoc = glGetUniformLocation(shader->programHandle, "cameraPos");
	//glUniform3f(cameraPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	//// Light position and properties (currently only one lightsource)
	//lightPosLoc = glGetUniformLocation(shader->programHandle, "light.position");
	//lightAmbientLoc = glGetUniformLocation(shader->programHandle, "light.ambient");
	//lightDiffuseLoc = glGetUniformLocation(shader->programHandle, "light.diffuse");
	//lightSpecularLoc = glGetUniformLocation(shader->programHandle, "light.specular");

	//glUniform3f(lightPosLoc, lightPositionInWorldSpace.x, lightPositionInWorldSpace.y, lightPositionInWorldSpace.z);
	//glUniform3f(lightAmbientLoc, 0.2f, 0.2f, 0.2f);
	//glUniform3f(lightDiffuseLoc, 0.5f, 0.5f, 0.5f);
	//glUniform3f(lightSpecularLoc, 1.0f, 1.0f, 1.0f);
	//startCube->draw();

	//// Mid Cube
	//auto& model_mid_cube = midCube->modelMatrix;
	//auto model_location_mid_cube = glGetUniformLocation(shader->programHandle, "model");
	//glUniformMatrix4fv(model_location_moving_cube, 1, GL_FALSE, glm::value_ptr(model_mid_cube));
	//midCube->draw();

	//// Finish Cube
	//auto& model_finish_cube = finishCube->modelMatrix;
	//auto model_location_finish_cube = glGetUniformLocation(shader->programHandle, "model");
	//glUniformMatrix4fv(model_location_moving_cube, 1, GL_FALSE, glm::value_ptr(model_finish_cube));
	//finishCube->draw();

	PhysXCube:
	PxRigidActor* box = physXCube->actor;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	while (nShapes--) {
		//should switch geometry type here!
		PxShape* shape = shapes[nShapes];
		PxMat44 shapePose(PxShapeExt::getGlobalPose(*shape, *box));

		//changed the y-coordinates to make it upside down
		glm::mat4 model_physXCube = glm::mat4(
			shapePose.column0.x, shapePose.column0.y, shapePose.column0.z, shapePose.column0.w,
			shapePose.column1.x, shapePose.column1.y, shapePose.column1.z, shapePose.column1.w,
			shapePose.column2.x, shapePose.column2.y, shapePose.column2.z, shapePose.column2.w,
			shapePose.column3.x, shapePose.column3.y, shapePose.column3.z, shapePose.column3.w);

		auto model_location_physXCube = glGetUniformLocation(basicShader->programHandle, "model");
		glUniformMatrix4fv(model_location_physXCube, 1, GL_FALSE, glm::value_ptr(model_physXCube));
		physXCube->draw();
	}
	delete[] shapes;

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

	// Particle system(s)
	glBindTexture(GL_TEXTURE_2D, particle);

	// Update particle system position when spaceship moves
	glm::vec3 newParticlePos = camera->getPosition() + camera->getFront() * 2.2f + vec3(0, 1.4f, 0);
	spaceship->particleSystem.UpdateParticleGenerationPosition(newParticlePos);

	spaceship->particleSystem.SetMatrices(&projection, camera->getPosition(), camera->getPosition() + camera->getFront(), camera->getUp());
	spaceship->particleSystem.UpdateParticles(time_delta);
	spaceship->particleSystem.RenderParticles();


	/* HUD */
	//glm::mat4 hudProjectionMat = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
	//hudShader->useShader();
	//glUniformMatrix4fv(glGetUniformLocation(hudShader->programHandle, "projection"), 1, GL_FALSE, glm::value_ptr(hudProjectionMat));
	///* If user is outside boundary(40x40x70) display warning */
	//if (camera.get()->getPosition().x < -20 
	//	|| camera.get()->getPosition().x > 20 
	//	|| camera.get()->getPosition().y < -20
	//	|| camera.get()->getPosition().y > 20
	//	|| camera.get()->getPosition().z < -10
	//	|| camera.get()->getPosition().z > 60) {
	//		renderText("Danger Zone.", 550.0f, 380.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
	//}
	//if (camera.get()->getPosition().z > 25) {
	//	renderText("You win!", 550.0f, 380.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
	//}


}

void cleanup() {
	basicShader.reset(nullptr);
	/* Light Cube */
	//lightCube->particleSystem.~ParticleTF();
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
	/* PhysX Cube */
	physXCube.reset(nullptr);
	/*Framebuffer*/
	glDeleteTextures(2, texColorBuffer);
	glDeleteFramebuffers(1, &framebuffer);
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

	// Load particle
	particle = textureLoader->load("Textures/particle.bmp");

	//Load LensFlares color
	lensflaresColor = textureLoader->load("Textures/lensflares_color.png");
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

/*initalizes PhysX
used Tutorial (http://mmmovania.blogspot.co.at/2011/05/simple-bouncing-box-physx3.html) and PhysX Documentation (http://docs.nvidia.com/gameworks/content/gameworkslibrary/physx/guide/Index.html)*/
void initializePhysX() {
	gFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, gAllocator, gErrorCallback);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true);

	if (gPhysics == NULL) {
		std::cerr << "Error creating PhysX device." << endl;
		glfwTerminate;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.8f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);

	if (!gScene) {
		std::cerr << "create Scene failed!" << endl;
	}

	//Wut?
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.5f);

	//Create actors:
	//Create ground plane:
	PxRigidStatic* plane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 2), *gMaterial);
	if (!plane) {
		std::cerr << "create plane failed" << endl;
	}
	gScene->addActor(*plane);
}

/*shuts down PhysX*/
void shutdownPhysX() {
	gPhysics->release();
	gFoundation->release();
}

bool stepPhysX(float dt) {
	gAccumulator += dt;
	if (gAccumulator < gStepSize) {
		return false;
	}

	gAccumulator -= gStepSize;

	gScene->simulate(gStepSize);

	gScene->fetchResults(true);

	return true;
}