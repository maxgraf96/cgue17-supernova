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
	/* TODO: Translate instead of rotate */
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.01f));
}

void MovingCube::draw() {
	Cube::draw();
}

#endif // SCENE_OBJECT