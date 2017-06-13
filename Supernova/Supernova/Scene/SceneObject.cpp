#include "SceneObject.hpp"

using namespace supernova::scene;

SceneObject::SceneObject(glm::mat4& _modelMatrix)
	: modelMatrix(_modelMatrix) {}

SceneObject::~SceneObject() { }


glm::vec3 SceneObject::getPosition() {
	return glm::vec3(modelMatrix[3][0], modelMatrix[3][1], modelMatrix[3][2]);
}

glm::vec3 SceneObject::getPositionFromModelMatrix(glm::mat4 &_modelMatrix) {
	return glm::vec3(_modelMatrix[3][0], _modelMatrix[3][1], _modelMatrix[3][2]);
}