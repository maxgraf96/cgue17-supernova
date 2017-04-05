#include "SceneObject.hpp"

using namespace supernova::scene;

SceneObject::SceneObject(glm::mat4& _modelMatrix)
	: modelMatrix(_modelMatrix) {}

SceneObject::~SceneObject() {}