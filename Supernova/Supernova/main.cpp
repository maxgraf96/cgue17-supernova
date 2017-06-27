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
#include "Shader.hpp"
#include "Scene\CollisionDetection\BoundingSphere.hpp"
#include "Scene\CollisionDetection\AABB.hpp"
#include "Scene\Laser.hpp"
//#include "Particles\ParticleTF.hpp"
#include "Particles\ExtPTF.hpp"
#include "Lights\Light.hpp"
#include "Frustum.hpp"

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
std::unique_ptr<LightCube> lightCube;
std::unique_ptr<Sun> sun;
std::unique_ptr<Laser> laser;
std::unique_ptr<Radar> radar;
std::unique_ptr<Asteroid> asteroid1;
std::unique_ptr<Asteroid> asteroid2;
std::unique_ptr<Asteroid> asteroid3;
std::unique_ptr<Asteroid> asteroid4;
std::unique_ptr<Asteroid> asteroid5;
std::unique_ptr<Asteroid> asteroid6;
std::unique_ptr<Asteroid> asteroid7;
std::unique_ptr<Asteroid> asteroid8;
std::unique_ptr<Asteroid> asteroid9;
std::unique_ptr<Asteroid> asteroid10;

int asteroidCount = 0;
bool won = false;
bool lost = false;
int mins = 3;
int sec = 0;
float buffer = 0;
int bloomStrength = 2;
int countForBloom = 0;

// Camera
std::unique_ptr<Camera> camera;

//Frustum
Frustum viewFrustum = Frustum();

// Textuer loader
std::unique_ptr<TextureLoader> textureLoader = std::make_unique<TextureLoader>();

// Textures
GLuint cubeMapTexture;
GLuint particle;
GLuint lensflaresColor;

//Framebuffer
GLuint framebuffer;
GLuint texColorBuffer[2];
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

/* vectors that contain light sources. They are subsequently passed to all shaders of objects that are to be lit. lit. */
std::vector<PointLight> pointLights;
std::vector<DirectionalLight> dirLights;
std::vector<SpotLight> spotLights;

// Lights
PointLight cubeLight;
SpotLight headLights;

//camera
glm::mat4 view;
glm::mat4 projection;

// Radar
bool radarRotationDirection = false;
bool radarRetracting = false;
bool radarRetracted = false;

int width = 800;
int height = 600;
bool fullscreen = false;

float time_delta = 0;

//for enabling features
bool frustumCulling = false;
bool frameTimeDisplay = false;
bool wireFrame = false;
bool textureSampling = true;// True bilinear, false nearest neighbor
int mipMappingQuality = 0; // 0 Off, 1 Nearest neighbor, 2 linear
bool blending = false;

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

	while (running && !glfwWindowShouldClose(window)) {

		//clear frame and depth buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//compute frame time delta
		auto time_new = glfwGetTime();
		time_delta = (float)(time_new - time);
		time = time_new;

		buffer = buffer + time_delta;
		if (buffer >= 1) {
			buffer = buffer - 1;
			if (sec == 0) {
				sec = 60;
				mins = mins - 1;
			}
			else {
				sec = sec - 1;
			}
			countForBloom = countForBloom + 1;
			if (countForBloom == 2) {
				countForBloom = 0;
				bloomStrength = bloomStrength + 1;
			}
		}

		if (mins == 0 && sec == 0) {
			lost = true;
		}

		/* Camera position console output */
		std::cout << "frametime:" << time_delta * 1000 << "ms =~" << 1.0 / time_delta << "fps" << std::endl;

		//react to user input
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
		else if (glfwGetKey(window, GLFW_KEY_Q)) {
			radarRotationDirection = false;
		}
		else if (glfwGetKey(window, GLFW_KEY_E)) {
			radarRotationDirection = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_U)) {
			radarRetracting = true;
		}
		else if (glfwGetKey(window, GLFW_KEY_F2)) {
			frameTimeDisplay = !frameTimeDisplay;
		}
		else if (glfwGetKey(window, GLFW_KEY_F3)) {
			wireFrame = !wireFrame;
		}
		else if (glfwGetKey(window, GLFW_KEY_F9)) {
			blending = !blending;
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
		if (glfwGetKey(window, GLFW_KEY_F8)) {
			frustumCulling = !frustumCulling;
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

		//check if left mouse button was clicked for shooting the laser
		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			laser->setShooting(true);
		}
		else {
			laser->setShooting(false);
		}
		
		vector<BoundingSphere*> obstacles;

		BoundingSphere boundingSun = sun->boundingSphere;
		boundingSun.setPosition(sun->getPosition());
		obstacles.push_back(&boundingSun);

		asteroidCount = 0;

		if (!asteroid1->getDestroyed()) {
			BoundingSphere boundingAsteroid1 = asteroid1->boundingSphere;
			boundingAsteroid1.setPosition(asteroid1->getPosition());
			obstacles.push_back(&boundingAsteroid1);

			asteroidCount++;
		}
		if (!asteroid2->getDestroyed()) {
			BoundingSphere boundingAsteroid2 = asteroid2->boundingSphere;
			boundingAsteroid2.setPosition(asteroid2->getPosition());
			obstacles.push_back(&boundingAsteroid2);

			asteroidCount++;
		}
		if (!asteroid3->getDestroyed()) {
			BoundingSphere boundingAsteroid3 = asteroid3->boundingSphere;
			boundingAsteroid3.setPosition(asteroid3->getPosition());
			obstacles.push_back(&boundingAsteroid3);

			asteroidCount++;
		}
		if (!asteroid4->getDestroyed()) {
			BoundingSphere boundingAsteroid4 = asteroid4->boundingSphere;
			boundingAsteroid4.setPosition(asteroid4->getPosition());
			obstacles.push_back(&boundingAsteroid4);

			asteroidCount++;
		}
		if (!asteroid5->getDestroyed()) {
			BoundingSphere boundingAsteroid5 = asteroid5->boundingSphere;
			boundingAsteroid5.setPosition(asteroid5->getPosition());
			obstacles.push_back(&boundingAsteroid5);

			asteroidCount++;
		}
		if (!asteroid6->getDestroyed()) {
			BoundingSphere boundingAsteroid6 = asteroid6->boundingSphere;
			boundingAsteroid6.setPosition(asteroid6->getPosition());
			obstacles.push_back(&boundingAsteroid6);

			asteroidCount++;
		}
		if (!asteroid7->getDestroyed()) {
			BoundingSphere boundingAsteroid7 = asteroid7->boundingSphere;
			boundingAsteroid7.setPosition(asteroid7->getPosition());
			obstacles.push_back(&boundingAsteroid7);

			asteroidCount++;
		}
		if (!asteroid8->getDestroyed()) {
			BoundingSphere boundingAsteroid8 = asteroid8->boundingSphere;
			boundingAsteroid8.setPosition(asteroid8->getPosition());
			obstacles.push_back(&boundingAsteroid8);

			asteroidCount++;
		}
		if (!asteroid9->getDestroyed()) {
			BoundingSphere boundingAsteroid9 = asteroid9->boundingSphere;
			boundingAsteroid9.setPosition(asteroid9->getPosition());
			obstacles.push_back(&boundingAsteroid9);

			asteroidCount++;
		}
		if (!asteroid10->getDestroyed()) {
			BoundingSphere boundingAsteroid10 = asteroid10->boundingSphere;
			boundingAsteroid10.setPosition(asteroid10->getPosition());
			obstacles.push_back(&boundingAsteroid10);

			asteroidCount++;
		}


		if (asteroidCount == 0) {
			won = true;
		}

		//update spaceship
		spaceship->update(forward, backward, rollLeft, rollRight, xoffset, yoffset, time_delta, obstacles);
		radar->update(time_delta, pressed, spaceship->modelMatrix, radarRotationDirection, radarRetracting, radarRetracted);
		laser->update(time_delta, pressed, spaceship->modelMatrix, camera->getFront());

		//update camera
		glm::vec3  cameraFront = spaceship->front;
		glm::vec3  cameraUp = spaceship->up * -1.0f;
		glm::vec3  cameraRight = spaceship->right;
		glm::vec3 cameraPos = spaceship->getPosition();
		cameraPos = cameraPos - 12.0f * cameraFront;
		cameraPos = cameraPos - 2.0f * cameraUp;

		camera->update(cameraPos, cameraFront, cameraUp, cameraRight);

		update(time_delta, pressed);

		viewFrustum.update(camera->getPosition(), camera->getFront(), camera->getUp());

		if (laser->getShooting())
		{
			AABB boundingLaser = laser->boundingBox;

			asteroid1->detectHit(&boundingLaser, time_delta);
			asteroid2->detectHit(&boundingLaser, time_delta);
			asteroid3->detectHit(&boundingLaser, time_delta);
			asteroid4->detectHit(&boundingLaser, time_delta);
			asteroid5->detectHit(&boundingLaser, time_delta);
			asteroid6->detectHit(&boundingLaser, time_delta);
			asteroid7->detectHit(&boundingLaser, time_delta);
			asteroid8->detectHit(&boundingLaser, time_delta);
			asteroid9->detectHit(&boundingLaser, time_delta);
			asteroid10->detectHit(&boundingLaser, time_delta);
		}

		//generate view matrix
		view = camera->viewMatrix;

		/*****START RENDER*****/
		//based on the framebuffers tutorial from "Learn OpenGL" (https://learnopengl.com/#!Advanced-OpenGL/Framebuffers)

		//first pass, draw to framebuffer
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glEnable(GL_DEPTH_TEST); //could cause trouble
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		if (wireFrame)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		if (blending) {
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
		else {
			glDisable(GL_BLEND);
		}

		draw();
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		/******POST PROCESSING******/
		{
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
				glActiveTexture(GL_TEXTURE0);
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

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texColorBuffer[1]);
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
			previousVP = camera->viewMatrix * projection;// previous vp = current vp
		}

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
	radar = std::make_unique<Radar>(glm::mat4(1.0f), "Models/radar/radar.obj");
	spaceship = std::make_unique<Spaceship>(glm::mat4(1.0f), "Models/spaceship/spaceship.obj");

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

	laser = std::make_unique<Laser>(glm::mat4(1.0f), "Models/cube/cube.obj");

	//Create Asteroids
	asteroid1 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid1->modelMatrix = glm::translate(asteroid1->modelMatrix, glm::vec3(20.0f, 120.0f, -220.0f));

	asteroid2 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid2->modelMatrix = glm::translate(asteroid2->modelMatrix, glm::vec3(50.0f, 80.0f, -240.0f));

	asteroid3 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid3->modelMatrix = glm::translate(asteroid3->modelMatrix, glm::vec3(30.0f, 100.0f, -190.0f));

	asteroid4 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid4->modelMatrix = glm::translate(asteroid4->modelMatrix, glm::vec3(80.0f, -80.0f, 100.0f));

	asteroid5 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid5->modelMatrix = glm::translate(asteroid5->modelMatrix, glm::vec3(30.0f, -80.0f, 140.0f));

	asteroid6 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid6->modelMatrix = glm::translate(asteroid6->modelMatrix, glm::vec3(-40.0f, 180.0f, 20.0f));

	asteroid7 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid7->modelMatrix = glm::translate(asteroid7->modelMatrix, glm::vec3(-200.0f, 220.0f, 0.0f));

	asteroid8 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid8->modelMatrix = glm::translate(asteroid8->modelMatrix, glm::vec3(0.0f, -140.0f, 0.0f));

	asteroid9 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid9->modelMatrix = glm::translate(asteroid9->modelMatrix, glm::vec3(10.0f, -120.0f, 40.0f));

	asteroid10 = std::make_unique<Asteroid>(glm::mat4(1.0f), "Models/meteor/mymeteor.obj");
	asteroid10->modelMatrix = glm::translate(asteroid10->modelMatrix, glm::vec3(-50.0f, -180.0f, 180.0f));

	/* Create lights */
	/* DIRECTIONAL */
	sun = std::make_unique<Sun>(glm::mat4(1.0f), "Models/newsun/newsun.obj");
	vec3 sunLightColor = vec3(0.5f);
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


	/* POINT */
	/*vec3 lightCubeColor = vec3(0, 0, 0);
	lightCube = std::make_unique<LightCube>(glm::mat4(1.0f), lightCubeShader.get());
	cubeLight = PointLight(lightCubeColor, lightCubeColor, lightCubeColor, lightCube->getPosition(),
		1.0f, 0.9f, 0.2f);
	cubeLight.setInitialized(true);*/
	//pointLights.push_back(cubeLight);

	/* SPOT */
	vec3 color = vec3(1.0f);
	vec3 position = camera->getPosition() - 8.5f * camera->getFront();
	vec3 direction = camera->getFront();
	headLights = SpotLight(position, color, direction, glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.0f)));
	headLights.setInitialized(true);
	spotLights.push_back(headLights);
	
	
	/******SETUP FRAMEBUFFER******/
	{

		glGenFramebuffers(1, &framebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

		//generate two textures (colorbuffers) and attach them to framebuffer
		glGenTextures(2, texColorBuffer);
		for (GLuint i = 0; i < 2; i++) {
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
		GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		glDrawBuffers(2, attachments);

		//check if framebuffer is complete
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		/******SETUP PINGPONG FRAMEBUFFERS******/
		//based on the Bloom tutorial from "Learn OpenGL" (https://learnopengl.com/#!Advanced-Lighting/Bloom)
		glGenFramebuffers(2, pingpongFBO);
		glGenTextures(2, pingpongBuffer);
		for (GLuint i = 0; i < bloomStrength; i++) {
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

	}

	// HUD
	textQuad = std::make_unique<TextQuad>(glm::mat4(1.0f), hudShader.get());

	// Particle system(s)
	spaceship->particleSystem.InitalizeParticleSystem();
	spaceship->particleSystem.SetGeneratorProperties(
		spaceship->getPositionFromModelMatrix(glm::translate(spaceship->modelMatrix, vec3(0.0f, 0.0f, -8.5f))), // Where the particles are generated
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

	laser->particleSystem.InitalizeParticleSystem();
	laser->particleSystem.SetGeneratorProperties(
		laser->getPositionFromModelMatrix(glm::translate(laser->modelMatrix, vec3(0.0f, 0.0f, -0.9f))),
		glm::vec3(-0.1, -0.1, 80), // Minimal velocity
		glm::vec3(0.1, 0.1, 120), // Maximal velocity
		glm::vec3(0, 0, 0), // Gravity force applied to particles
		glm::vec3(1.0f, 0.0f, 0.0f), // Color
		1.0f, // Minimum lifetime in seconds
		2.0f, // Maximum lifetime in seconds
		0.5f, // Rendered size
		0.1f, // Spawn every x seconds
		20// And spawn 30 particles);
	);

	int width;
	int height;
	glfwGetWindowSize(window, &width, &height);

	/* glm::perspective takes (fov, aspect, nearPlane, farPlane) */
	projection = glm::perspective(30.0f, (float )width / (float)height, 0.1f, 2000.0f);
	viewFrustum.setCameraParameters(0.1f, 2000.0f, (float)width / (float)height, 30.0f);
	}

void update(float time_delta, int pressed) {
	skybox->update(time_delta, pressed);

	//lightCube->update(time_delta, pressed);
	startCube->update(time_delta, pressed);
	midCube->update(time_delta, pressed);
	finishCube->update(time_delta, pressed);

	textQuad->update(time_delta, pressed);
	sun->update(time_delta, pressed);

	textQuad->update(time_delta, pressed);

	// Update lights
	// point lights
	/*cubeLight.updatePosition(lightCube->getPosition());
	pointLights.clear();
	pointLights.push_back(cubeLight);*/

	// spot lights
	glm::vec3 newPos = camera->getPosition() + camera->getFront() * 12.0f;
	headLights.updatePosition(newPos);
	headLights.updateDirection(camera->getFront());
	spotLights.clear();
	spotLights.push_back(headLights);

	// Directional lights
	for (DirectionalLight& light : dirLights) {
		glm::vec3 newAmbient = light.getAmbient() + glm::vec3(time_delta * 0.00205f);
		glm::vec3 newDiffuse = light.getDiffuse() + glm::vec3(time_delta * 0.001025f);
		//glm::vec3 newSpecular = light.getSpecular() + glm::vec3(time_delta * 0.0082f);
		if (newAmbient.r > 1)
			newAmbient = glm::vec3(1.0f);
		if (newDiffuse.r > 1)
			newDiffuse = glm::vec3(1.0f);
		/*if (newSpecular.r > 1)
			newSpecular = glm::vec3(1.0f);*/

		light.setAmbient(newAmbient);
		light.setDiffuse(newDiffuse);
		//light.setSpecular(newSpecular);
	}
}

void draw() {
	//generate view projection matrix
	auto view_projection = projection * view;

	/*-------------------- Get light sources --------------------- */
	// Convert light vectors to arrays to pass them to shaders
	PointLight point_lights_array[20];
	DirectionalLight dir_lights_array[12];
	SpotLight spot_lights_array[12];
	//std::copy(pointLights.begin(), pointLights.end(), point_lights_array);
	std::copy(dirLights.begin(), dirLights.end(), dir_lights_array);
	std::copy(spotLights.begin(), spotLights.end(), spot_lights_array);

	/* Light Cube */
	{
		//lightCubeShader->useShader();
		//auto view_projection_location_light_cube = glGetUniformLocation(lightCubeShader->programHandle, "proj");
		//glUniformMatrix4fv(view_projection_location_light_cube, 1, GL_FALSE, glm::value_ptr(view_projection));

		//// make light cube a little bit smaller and put it schräg above other cube
		//auto& model_light_cube = glm::scale(lightCube->modelMatrix, glm::vec3(3));
		//model_light_cube = glm::translate(model_light_cube, glm::vec3(2.5f, -5.5f, -30));
		//auto model_location_light_cube = glGetUniformLocation(lightCubeShader->programHandle, "model");
		//glUniformMatrix4fv(model_location_light_cube, 1, GL_FALSE, glm::value_ptr(model_light_cube));
		//lightCube->draw();
	}


	// Laser cube
	{
		basicShader->useShader();
		basicShader->setLightSources(dir_lights_array, point_lights_array, spot_lights_array, camera.get());
		auto& model_laser = laser->modelMatrix;
		auto model_location_laser = glGetUniformLocation(basicShader->programHandle, "model");
		glUniformMatrix4fv(model_location_laser, 1, GL_FALSE, glm::value_ptr(model_laser));
		auto view_projection_location_laser = glGetUniformLocation(basicShader->programHandle, "proj");
		glUniformMatrix4fv(view_projection_location_laser, 1, GL_FALSE, glm::value_ptr(view_projection));
		//laser->draw(basicShader.get());
	}


	// view(camera) info
	GLint cameraPosLoc;

	/*-------------------- Textured objects --------------------- */
	{
		textureShader->useShader();
		// SET ALL LIGHT SOURCES WITH THIS LIMITED EDITION ONE LINER
		textureShader->setLightSources(dir_lights_array, point_lights_array, spot_lights_array, camera.get());

		// Spaceship
		auto view_projection_location_spaceship = glGetUniformLocation(textureShader->programHandle, "proj");
		glUniformMatrix4fv(view_projection_location_spaceship, 1, GL_FALSE, glm::value_ptr(view_projection));
		auto& model_spaceship = spaceship->modelMatrix;
		auto model_location_spaceship = glGetUniformLocation(textureShader->programHandle, "model");
		glUniformMatrix4fv(model_location_spaceship, 1, GL_FALSE, glm::value_ptr(model_spaceship));
		spaceship->draw(textureShader.get());

		// Radar
		auto view_projection_location_radar = glGetUniformLocation(textureShader->programHandle, "proj");
		glUniformMatrix4fv(view_projection_location_radar, 1, GL_FALSE, glm::value_ptr(view_projection));
		auto& model_radar = radar->modelMatrix;
		auto model_location_radar = glGetUniformLocation(textureShader->programHandle, "model");
		glUniformMatrix4fv(model_location_radar, 1, GL_FALSE, glm::value_ptr(model_radar));
		radar->draw(textureShader.get());

	}

	//View Frustum Culling:
	BoundingSphere boundingSun = sun->boundingSphere;
	boundingSun.setPosition(sun->getPosition());

	if (!frustumCulling || !(viewFrustum.testSphere(&boundingSun) == viewFrustum.OUTSIDE)) {
		// Sun
		sunShader->useShader();
		auto view_projection_location_sun = glGetUniformLocation(sunShader->programHandle, "proj");
		glUniformMatrix4fv(view_projection_location_sun, 1, GL_FALSE, glm::value_ptr(view_projection));
		auto& model_sun = sun->modelMatrix;
		auto model_location_sun = glGetUniformLocation(sunShader->programHandle, "model");
		glUniformMatrix4fv(model_location_sun, 1, GL_FALSE, glm::value_ptr(model_sun));
		sun->draw(sunShader.get());
	}

	basicShader->useShader();
	/*Asteroids*/
	{
		auto view_projection_location_asteroid = glGetUniformLocation(basicShader->programHandle, "proj");
		glUniformMatrix4fv(view_projection_location_asteroid, 1, GL_FALSE, glm::value_ptr(view_projection));

		BoundingSphere boundingAsteroid1 = asteroid1->boundingSphere;
		boundingAsteroid1.setPosition(asteroid1->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid1) == viewFrustum.OUTSIDE)) {
			if (!asteroid1->getDestroyed()) {
				auto& model_asteroid1 = asteroid1->modelMatrix;
				auto model_location_asteroid1 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid1, 1, GL_FALSE, glm::value_ptr(model_asteroid1));

				asteroid1->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid2 = asteroid2->boundingSphere;
		boundingAsteroid2.setPosition(asteroid2->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid2) == viewFrustum.OUTSIDE)) {
			if (!asteroid2->getDestroyed()) {
				auto& model_asteroid2 = asteroid2->modelMatrix;
				auto model_location_asteroid2 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid2, 1, GL_FALSE, glm::value_ptr(model_asteroid2));

				asteroid2->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid3 = asteroid3->boundingSphere;
		boundingAsteroid3.setPosition(asteroid3->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid3) == viewFrustum.OUTSIDE)) {
			if (!asteroid3->getDestroyed()) {
				auto& model_asteroid3 = asteroid3->modelMatrix;
				auto model_location_asteroid3 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid3, 1, GL_FALSE, glm::value_ptr(model_asteroid3));

				asteroid3->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid4 = asteroid4->boundingSphere;
		boundingAsteroid4.setPosition(asteroid4->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid4) == viewFrustum.OUTSIDE)) {
			if (!asteroid4->getDestroyed()) {
				auto& model_asteroid4 = asteroid4->modelMatrix;
				auto model_location_asteroid4 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid4, 1, GL_FALSE, glm::value_ptr(model_asteroid4));

				asteroid4->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid5 = asteroid5->boundingSphere;
		boundingAsteroid5.setPosition(asteroid5->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid5) == viewFrustum.OUTSIDE)) {
			if (!asteroid5->getDestroyed()) {
				auto& model_asteroid5 = asteroid5->modelMatrix;
				auto model_location_asteroid5 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid5, 1, GL_FALSE, glm::value_ptr(model_asteroid5));

				asteroid5->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid6 = asteroid6->boundingSphere;
		boundingAsteroid6.setPosition(asteroid6->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid6) == viewFrustum.OUTSIDE)) {
			if (!asteroid6->getDestroyed()) {
				auto& model_asteroid6 = asteroid6->modelMatrix;
				auto model_location_asteroid6 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid6, 1, GL_FALSE, glm::value_ptr(model_asteroid6));

				asteroid6->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid7 = asteroid7->boundingSphere;
		boundingAsteroid7.setPosition(asteroid7->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid7) == viewFrustum.OUTSIDE)) {
			if (!asteroid7->getDestroyed()) {
				auto& model_asteroid7 = asteroid7->modelMatrix;
				auto model_location_asteroid7 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid7, 1, GL_FALSE, glm::value_ptr(model_asteroid7));

				asteroid7->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid8 = asteroid8->boundingSphere;
		boundingAsteroid8.setPosition(asteroid8->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid8) == viewFrustum.OUTSIDE)) {
			if (!asteroid8->getDestroyed()) {
				auto& model_asteroid8 = asteroid8->modelMatrix;
				auto model_location_asteroid8 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid8, 1, GL_FALSE, glm::value_ptr(model_asteroid8));

				asteroid8->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid9 = asteroid9->boundingSphere;
		boundingAsteroid9.setPosition(asteroid9->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid9) == viewFrustum.OUTSIDE)) {
			if (!asteroid9->getDestroyed()) {
				auto& model_asteroid9 = asteroid9->modelMatrix;
				auto model_location_asteroid9 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid9, 1, GL_FALSE, glm::value_ptr(model_asteroid9));

				asteroid9->draw(basicShader.get());
			}
		}

		BoundingSphere boundingAsteroid10 = asteroid10->boundingSphere;
		boundingAsteroid10.setPosition(asteroid10->getPosition());

		if (!frustumCulling || !(viewFrustum.testSphere(&boundingAsteroid10) == viewFrustum.OUTSIDE)) {
			if (!asteroid10->getDestroyed()) {
				auto& model_asteroid10 = asteroid10->modelMatrix;
				auto model_location_asteroid10 = glGetUniformLocation(basicShader->programHandle, "model");
				glUniformMatrix4fv(model_location_asteroid10, 1, GL_FALSE, glm::value_ptr(model_asteroid10));

				asteroid10->draw(basicShader.get());
			}
		}
	}

	{
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
	}
	basicShader->useShader();

	/* Skybox - ALWAYS DRAW LAST!!!!!!!!!!!!!!!!!!!!!!!!!!!!!11elf
	* Only 2d objects are allowed to be drawn after skybox
	/* Change depth function so depth test passes when values are equal to depth buffers content */
	{
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
	}

	/****** HUD ******/
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glm::mat4 hudProjectionMat = glm::ortho(0.0f, static_cast<GLfloat>(width), 0.0f, static_cast<GLfloat>(height));
		hudShader->useShader();
		glUniformMatrix4fv(glGetUniformLocation(hudShader->programHandle, "projection"), 1, GL_FALSE, glm::value_ptr(hudProjectionMat));
		/* If user is outside boundary(40x40x70) display warning */
		/*if (camera.get()->getPosition().x < -20
			|| camera.get()->getPosition().x > 20
			|| camera.get()->getPosition().y < -20
			|| camera.get()->getPosition().y > 20
			|| camera.get()->getPosition().z < -10
			|| camera.get()->getPosition().z > 60) {
			renderText("Danger Zone.", 550.0f, 380.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
		}*/
		/*if (camera.get()->getPosition().z > 25) {
			renderText("You win!", 550.0f, 380.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
		}*/
		if (frameTimeDisplay) {
			renderText(to_string((int)(1.0f / time_delta)) + " FPS", 50.0f, 580.0f, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
		}

		renderText(to_string(asteroidCount), width - 0.05 * width, height - 0.1 * height, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);

		if (won) {
			renderText("You win!", width / 2 - 0.05 * width, height / 2 + 0.1 * height, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
		}
		else if (lost) {
			renderText("You lose!", width / 2 - 0.05 * width, height / 2 + 0.1 * height, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), characters);
		}
		else {
			if (sec < 10) {
				renderText(to_string(mins) + ":0" + to_string(sec), width / 2 - 0.025 * width, height - 0.1 * height, 1.0f, glm::vec3(1.0f), characters);
			}
			else {
				renderText(to_string(mins) + ":" + to_string(sec), width / 2 - 0.025 * width, height - 0.1 * height, 1.0f, glm::vec3(1.0f), characters);
			}
		}

		glDisable(GL_BLEND);
	}

	// Particle system(s)
	glBindTexture(GL_TEXTURE_2D, particle);

	// Spaceship auspuff
	{
		// Update particle system position when spaceship moves
		glm::vec3 newParticlePos = camera->getPosition() + camera->getFront() * 2.2f + camera->getUp() * 1.2f;
		spaceship->particleSystem.UpdateParticleGenerationPosition(newParticlePos);

		spaceship->particleSystem.SetMatrices(&projection, camera->getPosition(), camera->getPosition() + camera->getFront(), camera->getUp());
		spaceship->particleSystem.UpdateParticles(time_delta);
		spaceship->particleSystem.RenderParticles(true);
	}

	// Laser particles
	{
		if (laser->getShooting()) {
			laser->particleSystem.UpdateParticleGenerationPosition(laser->getPosition());
			laser->particleSystem.UpdateParticleDirection(camera->getFront() * 100.0f);
			laser->particleSystem.SetMatrices(&projection, camera->getPosition(), camera->getPosition() + camera->getFront(), camera->getUp());
			laser->particleSystem.UpdateParticles(time_delta);
			laser->particleSystem.RenderParticles(true);
		}
		else {
			laser->particleSystem.UpdateParticleGenerationPosition(laser->getPosition());
			laser->particleSystem.UpdateParticleDirection(camera->getFront() * 100.0f);
			laser->particleSystem.SetMatrices(&projection, camera->getPosition(), camera->getPosition() + camera->getFront(), camera->getUp());
			laser->particleSystem.UpdateParticles(time_delta);
			laser->particleSystem.RenderParticles(false);
		}
		////update laser when spaceship moves
		laser->boundingBox.calculateAABB(laser->meshes, laser->modelMatrix);
	}
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
	/*Framebuffers*/
	glDeleteTextures(1, &depthBuffer);
	glDeleteTextures(2, texColorBuffer);
	glDeleteFramebuffers(1, &framebuffer);
	glDeleteTextures(2, pingpongBuffer);
	glDeleteFramebuffers(2, pingpongFBO);
	glDeleteTextures(1, &lensFlaresBuffer);
	glDeleteFramebuffers(1, &lensFlaresFBO);
	glDeleteTextures(1, &motionBlurColorOut);
	glDeleteFramebuffers(1, &motionBlurFBO);
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