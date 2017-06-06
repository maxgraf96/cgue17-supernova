#pragma once
#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "../Shader.hpp"
#include "../Materials/Material.hpp"

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

namespace supernova {
	namespace scene {
		class Mesh {
		public:
			vector<Vertex> vertices;
			vector<unsigned int> indices;
			vector<Texture> textures;

			Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures, Material* _material);
			~Mesh();
			void draw(Shader* shader);
		private:
			GLuint vao, vbo, ebo;
			// Material, if no texture is specified
			Material* noTextureMaterial;
			void setup();
		};
	}
}