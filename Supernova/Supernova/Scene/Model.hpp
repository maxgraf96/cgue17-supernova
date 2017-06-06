#pragma once
#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SceneObject.hpp"
#include "Mesh.hpp"
#include "../Textures/TextureLoader.hpp"


namespace supernova {
	namespace scene {
		class Model : public SceneObject {
		public:
			Model();
			Model(glm::mat4& matrix, string const &path);
			virtual ~Model();
			virtual void update(float time_delta, int pressed);
			virtual void draw(Shader* shader);
			// Need bc extending SceneObject
			virtual void draw();
		private:
			TextureLoader textureLoader;
			vector<Mesh> meshes;
			string directory;
			vector<Texture> alreadyLoadedTextures;

			void loadModel(string const &path);
			void processNode(aiNode* node, const aiScene* scene);
			Mesh processMesh(aiMesh* mesh, const aiScene* scene);
			vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
		};
	}
}