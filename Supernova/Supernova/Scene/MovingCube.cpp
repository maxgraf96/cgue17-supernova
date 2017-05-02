#include "Cube.hpp"
#include "MovingCube.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace supernova;
using namespace supernova::scene;

#ifdef SCENE_OBJECT

MovingCube::MovingCube()
	: Cube() {

}

MovingCube::MovingCube(glm::mat4& matrix, Shader* _shader, Material* _material, float zOffset)
	: Cube(matrix, _shader, _material) {
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, zOffset));
}

MovingCube::~MovingCube(){
	Cube::~Cube();
}

void MovingCube::update(float time_delta, int pressed) {
	float x = modelMatrix[3].x;
	float y = modelMatrix[3].y;
	float speed = 0.15f;

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
}

void MovingCube::draw() {
	Cube::draw();
}

#endif // SCENE_OBJECT