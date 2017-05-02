#include <iostream>
#include <glm\gtx\rotate_vector.hpp>
#include "Camera.hpp"

using namespace supernova;
using namespace supernova::scene;

Camera::Camera(glm::vec3 _position, glm::vec3 _front, glm::vec3 _up, glm::vec3 _right) :
	position(_position), front(_front), up(_up), right(_right), cameraSpeed(5.0f), rotateSpeed(90.0f), sensitivity(0.2f),
	totalPitch(0.0f), totalYaw(0.0f) {
	viewMatrix = glm::lookAt(position, position + front, up);
}

Camera::~Camera() {

}

void Camera::update(bool forward, bool backwards, bool rollLeft, bool rollRight, float xoffset, float yoffset,
	float time_delta) {
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	if (forward) {
		position += (cameraSpeed * time_delta) * front;
		if (totalPitch < 0.0f) {
			front = glm::rotate(front, glm::radians(totalPitch * 2 * time_delta), right);
			up = glm::rotate(up, glm::radians(totalPitch * 2 * time_delta), right);
		}
		if (totalPitch > 0.0f) {
			front = glm::rotate(front, glm::radians(totalPitch * 2 * time_delta), right);
			up = glm::rotate(up, glm::radians(totalPitch * 2 * time_delta), right);
		}
	}
	if (backwards) {
		position -= (cameraSpeed * time_delta) * front;
	}
	if (!(rollLeft && rollRight)) {
		if (rollLeft) {
			roll = rotateSpeed * time_delta;
		}
		if (rollRight) {
			roll = -(rotateSpeed * time_delta);
		}
	}

	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw = xoffset;
	pitch = -yoffset;
	totalYaw += yaw;
	totalPitch += pitch;
	
	if (totalPitch > 25.0f || totalPitch < -25.0f) {
		totalPitch -= pitch;
	}
	else {
		front = glm::normalize(front * cos(glm::radians(pitch)) + up * sin(glm::radians(pitch)));
		up = glm::normalize(glm::cross(right, front));
	}

	if (totalYaw > 25.0f || totalYaw < -25.0f) {
		totalYaw -= yaw;
	}
	else {
		right = glm::normalize(right * cos(glm::radians(yaw)) + front * sin(glm::radians(yaw)));
		front = glm::normalize(glm::cross(up, right));
	}

	right = glm::normalize(right * cos(glm::radians(roll)) + up * sin(glm::radians(roll)));
	up = glm::normalize(glm::cross(right, front));

	viewMatrix = glm::lookAt(position, position + front, up);
}