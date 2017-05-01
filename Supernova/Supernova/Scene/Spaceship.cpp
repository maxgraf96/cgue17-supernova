#include "Spaceship.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace supernova;
using namespace supernova::scene;

Spaceship::Spaceship()
	: SceneObject(glm::mat4(1.0f)) {
}

Spaceship::Spaceship(glm::mat4& matrix, Shader* _shader, Material* _material)
	: SceneObject(matrix), shader(_shader), material(_material), front(glm::vec3(0.0f, 0.0f, 1.0f)), up(glm::vec3(0.0f, 1.0f, 0.0f)),
	right(glm::cross(front, up)), totalYaw(0.0f), totalPitch(0.0f) {
	//Load data to buffer
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, SPACESHIP_VERTEX_COUNT * sizeof(glm::vec3), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, SPACESHIP_INDEX_COUNT * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, SPACESHIP_VERTEX_COUNT * 3 * sizeof(float), normals, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//Generate bindings
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Position
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	auto positionIndex = glGetAttribLocation(shader->programHandle, "position");
	glEnableVertexAttribArray(positionIndex);
	glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Normals
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	GLint normalIndex = glGetAttribLocation(shader->programHandle, "normal");
	glEnableVertexAttribArray(normalIndex);
	glVertexAttribPointer(normalIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// Index
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	//unbind VAO first, then VBOs
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Spaceship::~Spaceship() {
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vao);
}

void Spaceship::update(float time_delta, int pressed) {

}

void Spaceship::update(float time_delta, int pressed, bool forward, bool backward, bool rollLeft, bool rollRight, GLfloat xoffset, GLfloat yoffset) {
	float roll = 0.0f;
	float pitch = 0.0f;
	float yaw = 0.0f;
	if (forward) {
		modelMatrix = glm::translate(modelMatrix, (5.0f * time_delta) * front);
	}
	if (backward) {
		modelMatrix = glm::translate(modelMatrix, -(5.0f * time_delta) * front);
	}
	if (!(rollLeft && rollRight)) {
		if (rollLeft) {
			modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f * time_delta), front);
			roll = -(90.0f * time_delta);
		}
		if (rollRight) {
			modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f * time_delta), front);
			roll = 90.0f * time_delta;
		}
	}

	xoffset *= 0.2f;
	yoffset *= 0.2f;

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
}

void Spaceship::draw() {
	glBindVertexArray(vao);

	/* Pass material values to shader */
	GLint matAmbientLoc = glGetUniformLocation(shader->programHandle, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(shader->programHandle, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shader->programHandle, "material.specular");
	GLint matShininessLoc = glGetUniformLocation(shader->programHandle, "material.shininess");
	GLint lightColorLoc = glGetUniformLocation(shader->programHandle, "lightColor");
	glUniform3f(matAmbientLoc, material->getAmbient().r, material->getAmbient().g, material->getAmbient().b);
	glUniform3f(matDiffuseLoc, material->getAmbient().r, material->getAmbient().g, material->getAmbient().b);
	glUniform3f(matSpecularLoc, material->getSpecular().r, material->getSpecular().g, material->getSpecular().b);
	glUniform1f(matShininessLoc, material->getShininess());
	glUniform3f(lightColorLoc, 1.0f, 1.0f, 1.0f);// Normally light is white

	glDrawElements(GL_TRIANGLES, SPACESHIP_INDEX_COUNT, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

const float Spaceship::positions[SPACESHIP_VERTEX_COUNT * 3] = {
	//Back
	-0.5f, 1.0f, -4.0f,
	0.5f, 1.0f, -4.0f,
	0.0f, 0.0f, -4.0f,

	//Front
	-0.5f, 1.0f, -2.0f,
	0.5f, 1.0f, -2.0f,
	0.0f, 0.0f, -2.0f,

	//Bottom
	-0.5f, 1.0f, -4.0f,
	0.5f, 1.0f, -4.0f,
	-0.5f, 1.0f, -2.0f,
	0.5f, 1.0f, -2.0f,

	//Right
	-0.5f, 1.0f, -4.0f,
	0.0f, 0.0f, -4.0f,
	-0.5f, 1.0f, -2.0f,
	0.0f, 0.0f, -2.0f,

	//Left
	0.5f, 1.0f, -4.0f,
	0.0f, 0.0f, -4.0f,
	0.5f, 1.0f, -2.0f,
	0.0f, 0.0f, -2.0f
};

const unsigned int Spaceship::indices[SPACESHIP_INDEX_COUNT] = {
	//Back
	0, 1, 2,

	//Front
	3, 4, 5,

	//Bottom
	6, 7, 9,
	6, 9, 8,

	//Left
	10, 11, 13,
	10, 13, 12,

	//Right
	14, 15, 17,
	14, 17, 16
};

const float Spaceship::normals[SPACESHIP_VERTEX_COUNT * 3] = {
	//Back
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	//Front
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	//Bottom
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	//Left
	-0.9f, 0.4f, 0.0f,
	-0.9f, 0.4f, 0.0f,
	-0.9f, 0.4f, 0.0f,
	-0.9f, 0.4f, 0.0f,

	//Right
	0.9f, 0.4f, 0.0f,
	0.9f, 0.4f, 0.0f,
	0.9f, 0.4f, 0.0f,
	0.9f, 0.4f, 0.0f,
};
