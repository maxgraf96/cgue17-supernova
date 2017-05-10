#pragma once
#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Shader.hpp"

// Never forget
using namespace supernova;
using namespace std;

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 textureCoordinates;
};

struct Texture {
	GLuint id;
	string type;
	aiString path;
};

class Mesh {
	public:
		vector<Vertex> vertices;
		vector<GLuint> indices;
		vector<Texture> textures;

		Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
		~Mesh();
		void draw(Shader shader);
	private: 
		GLuint vao, vbo, ebo;
		void setup();
};