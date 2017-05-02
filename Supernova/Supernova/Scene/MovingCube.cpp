#include "Cube.hpp"
#include "MovingCube.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace supernova;
using namespace supernova::scene;

#ifdef SCENE_OBJECT

MovingCube::MovingCube()
	: Cube() {

}

MovingCube::MovingCube(glm::mat4& matrix, Shader* _shader, Camera* _camera, Material* _material, float _zOffset)
	: Cube(matrix, _shader, _material), zOffset(_zOffset), camera(_camera) {
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, zOffset));
}

MovingCube::~MovingCube(){
	Cube::~Cube();
}

void MovingCube::update(float time_delta, int pressed) {
	float x = modelMatrix[3].x;
	float y = modelMatrix[3].y;
	float speed = 10.0f * time_delta;

	if (x <= 10 && y >= 0) {
		// Move right
		modelMatrix = glm::translate(modelMatrix, glm::vec3(speed, 0.0f, 0.0f));
	}
	if (x >= 10 && y < 10) {
		// Move up
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -speed, 0.0f));
	}
	if (y <= -10 && x > -10) {
		// Move left
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-speed, 0.0f, 0.0f));
	}
	if (x <= -10 && y < 0) {
		// Move down
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, speed, 0.0f));
	}

	// Color green if passed
	if (camera->getPosition().z > modelMatrix[3].z &&  this->getMaterial()->getAmbient().x != 0.678431f) {
		this->setMaterial(new Metal(vec3(0.678431f, 1.0f, 0.184314f)));
	}
}

void MovingCube::draw() {
	Cube::draw();
}

#endif // SCENE_OBJECT