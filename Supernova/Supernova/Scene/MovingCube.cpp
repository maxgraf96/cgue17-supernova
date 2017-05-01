#include "Cube.hpp"
#include "MovingCube.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace supernova;
using namespace supernova::scene;

#ifdef SCENE_OBJECT

MovingCube::MovingCube()
	: Cube() {

}

MovingCube::MovingCube(glm::mat4& matrix, Shader* _shader, Material* _material)
	: Cube(matrix, _shader, _material) {
}

MovingCube::~MovingCube(){
	Cube::~Cube();
}

void MovingCube::update(float time_delta, int pressed) {
	bool right = modelMatrix[3].x < 10;
	bool forward = modelMatrix[3].z < 10;
	bool back = modelMatrix[3].x < -10;
	if (right) {
		if (!forward) {
			if (back) {
				modelMatrix = glm::translate(modelMatrix, glm::vec3(0.00f, 0.0f, -0.05f));
			}
			else {
				modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.05f, 0.0f, 0.0f));
			}
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.05f, 0, 0));
		}
	}
	if (!right) {
		if (!forward) {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.05f, 0.0f, 0.0f));
		}
		else {
			modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, 0.05f));
		}
	}
}

void MovingCube::draw() {
	Cube::draw();
}

#endif // SCENE_OBJECT