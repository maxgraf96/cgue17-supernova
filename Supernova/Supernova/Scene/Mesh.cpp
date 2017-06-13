#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <sstream>
#include <iostream>

#include "../Shader.hpp"
#include "Mesh.hpp"

using namespace std;
using namespace supernova;
using namespace supernova::scene;

Mesh::Mesh(vector<Vertex> _vertices, vector<unsigned int> _indices, vector<Texture> _textures, Material* _material) {
	this->vertices = _vertices;
	this->indices = _indices;
	this->textures = _textures;
	this->noTextureMaterial = _material;

	this->setup();
}

Mesh::~Mesh() {}

void Mesh::draw(Shader* shader) {
	unsigned int diffuseNr = 1;
	unsigned int specNr = 1;

	// If the mesh has a texture use it - uses the textureShader
	if (this->textures.size() > 0) {
		for (GLuint i = 0; i < this->textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);

			string name = this->textures[i].type;
			glUniform1i(glGetUniformLocation(shader->programHandle, ("material." + name).c_str()), i);
			glBindTexture(GL_TEXTURE_2D, this->textures[i].id);

		}
	}
	// Else use the color specified in the material - uses the basic shader
	else {
		/* Pass material values to shader */
		GLint matAmbientLoc = glGetUniformLocation(shader->programHandle, "material.ambient");
		GLint matDiffuseLoc = glGetUniformLocation(shader->programHandle, "material.diffuse");
		GLint matSpecularLoc = glGetUniformLocation(shader->programHandle, "material.specular");
		GLint matShininessLoc = glGetUniformLocation(shader->programHandle, "material.shininess");
		glUniform3f(matAmbientLoc, noTextureMaterial->getAmbient().r, noTextureMaterial->getAmbient().g, noTextureMaterial->getAmbient().b);
		glUniform3f(matDiffuseLoc, noTextureMaterial->getAmbient().r, noTextureMaterial->getAmbient().g, noTextureMaterial->getAmbient().b);
		glUniform3f(matSpecularLoc, noTextureMaterial->getSpecular().r, noTextureMaterial->getSpecular().g, noTextureMaterial->getSpecular().b);
		glUniform1f(matShininessLoc, noTextureMaterial->getShininess());
	}
	// Set shininess -> make changeable later
	glUniform1f(glGetUniformLocation(shader->programHandle, "material.shininess"), 16.0f);

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
		&vertices[0], GL_STATIC_DRAW);

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