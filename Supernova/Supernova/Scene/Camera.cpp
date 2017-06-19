#include <iostream>
#include <glm\gtx\rotate_vector.hpp>
#include "Camera.hpp"

using namespace supernova;
using namespace supernova::scene;

Camera::Camera(glm::vec3 _position, glm::vec3 _front, glm::vec3 _up, glm::vec3 _right) :
	position(_position), front(_front), up(_up), right(_right) {
	viewMatrix = glm::lookAt(position, position + front, up);
}

Camera::~Camera() {

}

void Camera::update(glm::vec3 _position, glm::vec3 _front, glm::vec3 _up, glm::vec3 _right) {
	position = _position;
	front = _front;
	up = _up;
	right = _right;

	viewMatrix = glm::lookAt(position, position + front, up);
}