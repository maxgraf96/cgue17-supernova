#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <sstream>

#include "../Shader.hpp"
#include "Mesh.hpp"

using namespace std;
using namespace supernova;

Mesh::Mesh(vector<Vertex> _vertices, vector<GLuint> _indices, vector<Texture> _textures)
	: vertices(_vertices), indices(_indices), textures(_textures) {
	this->setup();
}

Mesh::~Mesh() {}

void Mesh::draw(Shader shader) {
	GLuint diffuseNr = 1;
	GLuint specNr = 1;

	for (GLuint i = 0; i < this->textures.size(); i++) {
		glActiveTexture(GL_TEXTURE0 + i);

		// Get texture number
		stringstream ss;
		string nr;
		string name = this->textures[i].type;
		if (name == "texture_diffuse") {
			ss << diffuseNr++;
		}
		else {
			ss << specNr++;
		}
		nr = ss.str();

		glUniform1f(glGetUniformLocation(shader.programHandle, ("material." + name + nr).c_str()), i);
		glBindTexture(GL_TEXTURE_2D, this->textures[i].id);

	}
	glActiveTexture(GL_TEXTURE0);

	/* actual drawing of mesh */
	glBindVertexArray(this->vao);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	// unbind
	glBindVertexArray(0);
}

void Mesh::setup() {
	glGenVertexArrays(1, &this->vao);
	glGenBuffers(1, &this->vbo);
	glGenBuffers(1, &this->ebo);

	glBindVertexArray(this->vao);
	glBindBuffer(GL_ARRAY_BUFFER, this->vbo);

	glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
		&this->vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
		&this->indices[0], GL_STATIC_DRAW);

	/* vertex position */
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);

	/* vertex normals */
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, normal));// use offsetof to quickly point to normal data

	/* texture coordinates of vertex */
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
		(GLvoid*)offsetof(Vertex, textureCoordinates));

	/* unbind */
	glBindVertexArray(0);

}