#pragma once
#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../Shader.hpp"
#include "Mesh.hpp"

// Never forget
using namespace supernova;
using namespace std;

class Model {
	public:
		Model(const GLchar* path) {
			this->loadModel(path);
		}
		void draw(Shader shader);
	private:
		vector<Mesh> meshes;
		string directory;
		vector<Texture> alreadyLoadedTextures;

		void loadModel(string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};