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
			virtual void update(float time_delta, int pressed) = 0;
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

		/* --------------------------- Spaceship --------------------------- */
		class Spaceship
			: public supernova::scene::Model {

		public:
			Spaceship(glm::mat4& matrix, string const &path) : Model(matrix, path) {};
			void update(float time_delta, int pressed) override {

				// Movement
				if (pressed == 1) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.1f));
				}
				else if (pressed == -1) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, -0.1f));
				}
				else if (pressed == -2) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.1f, 0, 0));
				}
				else if (pressed == -3) {
					modelMatrix = glm::translate(modelMatrix, glm::vec3(0.1f, 0, 0));
				}

				// Update particle system position when spaceship moves
				glm::mat4 newParticlePos = glm::translate(modelMatrix, glm::vec3(0, 0, -8.5f));
				particleSystem.UpdateParticleGenerationPosition(getPositionFromModelMatrix(newParticlePos));
			}
		};

		/* --------------------------- Sun --------------------------- */
		class Sun
			: public supernova::scene::Model {

		public:
			Sun(glm::mat4& matrix, string const &path) : Model(matrix, path) {}
			void update(float time_delta, int pressed) override {
				// ...
			}
		};
	}
}