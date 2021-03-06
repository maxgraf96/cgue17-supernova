#include "Cube.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace supernova;
using namespace supernova::scene;

#ifdef SCENE_OBJECT

Cube::Cube()
	: SceneObject(glm::mat4(1.0f)) {

}

Cube::Cube(glm::mat4& matrix, Shader* _shader, Material* _material)
	: SceneObject(matrix), shader(_shader), material(_material) {
	//Load data to buffer
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, CUBE_VERTEX_COUNT * sizeof(glm::vec3), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, CUBE_INDEX_COUNT * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glGenBuffers(1, &normalsBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalsBuffer);
	glBufferData(GL_ARRAY_BUFFER, CUBE_VERTEX_COUNT * 3 * sizeof(float), normals, GL_STATIC_DRAW);
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

Cube::~Cube() {
	glDeleteBuffers(1, &positionBuffer);
	glDeleteBuffers(1, &indexBuffer);
	glDeleteVertexArrays(1, &vao);
}

void Cube::update(float time_delta, int pressed) {
	if (pressed == 1) {
		modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f) * time_delta, glm::vec3(0, 1, 0));
	}
	else if (pressed == -1) {
		modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f) * time_delta, glm::vec3(0, 1, 0));
	}
	else {
		modelMatrix = glm::rotate(modelMatrix, glm::radians(0.0f) * time_delta, glm::vec3(0, 1, 0));
	}
}

void Cube::draw() {
	glBindVertexArray(vao);

	/* Pass material values to shader */
	GLint matAmbientLoc = glGetUniformLocation(shader->programHandle, "material.ambient");
	GLint matDiffuseLoc = glGetUniformLocation(shader->programHandle, "material.diffuse");
	GLint matSpecularLoc = glGetUniformLocation(shader->programHandle, "material.specular");
	GLint matShininessLoc = glGetUniformLocation(shader->programHandle, "material.shininess");
	glUniform3f(matAmbientLoc, material->getAmbient().r, material->getAmbient().g, material->getAmbient().b);
	glUniform3f(matDiffuseLoc, material->getAmbient().r, material->getAmbient().g, material->getAmbient().b);
	glUniform3f(matSpecularLoc, material->getSpecular().r, material->getSpecular().g, material->getSpecular().b);
	glUniform1f(matShininessLoc, material->getShininess());

	glDrawElements(GL_TRIANGLES, CUBE_INDEX_COUNT, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

const float Cube::positions[CUBE_VERTEX_COUNT * 3] = {
	// Back
	-0.5f, -0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,

	// Front
	-0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, 0.5f,

	// Top
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, 0.5f, 0.5f,

	// Bottom
	0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, 0.5f,
	-0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,

	// Right
	0.5f, 0.5f, 0.5f,
	0.5f, -0.5f, 0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, 0.5f, -0.5f,

	// Left
	-0.5f, 0.5f, 0.5f,
	-0.5f, 0.5f, -0.5f,
	-0.5f, -0.5f, -0.5f,
	-0.5f, -0.5f, 0.5f


};

const unsigned int Cube::indices[CUBE_INDEX_COUNT] = {
	0,1,2,
	0,2,3,

	4,5,6,
	4,6,7,

	8,9,10,
	8,10,11,

	12,13,14,
	12,14,15,

	16,17,18,
	16,18,19,

	20,21,22,
	20,22,23
};

const float Cube::normals[CUBE_VERTEX_COUNT * 3] = {
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,

	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,
	0.0f, -1.0f, 0.0f,

	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,

	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
	-1.0f, 0.0f, 0.0f,
};
#endif // SCENE_OBJECT