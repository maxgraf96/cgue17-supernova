#include "Cube.hpp"

#include <glm/gtc/matrix_transform.hpp>

using namespace supernova;
using namespace supernova::scene;

#ifdef SCENE_OBJECT

Cube::Cube()
	: SceneObject(glm::mat4(1.0f)) {

}

Cube::Cube(glm::mat4& matrix, Shader* _shader)
	: SceneObject(matrix), shader(_shader) {
	//Load data to buffer
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, CUBE_VERTEX_COUNT * sizeof(glm::vec3), positions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, CUBE_INDEX_COUNT * sizeof(unsigned int), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	//Generate bindings
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	auto positionIndex = glGetAttribLocation(shader->programHandle, "position");
	glEnableVertexAttribArray(positionIndex);
	glVertexAttribPointer(positionIndex, 3, GL_FLOAT, GL_FALSE, 0, 0);

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

void Cube::update() {
	//TODO
}

void Cube::draw() {
	glBindVertexArray(vao);

	glDrawElements(GL_TRIANGLES, CUBE_INDEX_COUNT, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

const float Cube::positions[CUBE_VERTEX_COUNT * 3] = {
	0.5f, 0.5f, 0.5f,
	0.5f, 0.5f, -0.5f,
	0.5f, -0.5f, -0.5f,
	0.5f, -0.5f, 0.5f,

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

	0,1,4,
	1,5,4,

	2,3,6,
	3,7,6,

	1,2,6,
	1,6,5,

	0,4,7,
	0,7,3
};
#endif // SCENE_OBJECT