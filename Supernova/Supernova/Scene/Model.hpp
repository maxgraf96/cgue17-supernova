#pragma once
#include <vector>
#include<glm/glm.hpp>
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
			virtual void translate(glm::vec3 direction);
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
			vec3 front, up, right;

			Spaceship(glm::mat4& matrix, string const &path) : Model(matrix, path),
			speed(0.0f), rotateSpeed(90.0f), sensitivity(5.0f), totalPitch(0.0f), totalYaw(0.0f) {
				front = glm::vec3(0.0f, 0.0f, 1.0f);
				up = glm::vec3(0.0f, -1.0f, 0.0f);
				right = glm::cross(front, up);
			};

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
				glm::vec3 newParticlePos = getPosition() - 8.5f * front;
				particleSystem.UpdateParticleGenerationPosition(newParticlePos);
			}

			void update(bool forward, bool backward, bool rollLeft, bool rollRight, float xoffset, float yoffset,
				float time_delta) {
				glm::vec3 position = getPosition();
				float roll = 0.0f;
				float pitch = 0.0f;

				if (forward) {
					if (speed < 10.0f) {
						speed += 2.5f * time_delta;
					}
				}
				if (backward) {
					if (speed > 0.0f) {
						speed -= 2.5f * time_delta;
					}
				}

				if (!(rollLeft && rollRight)) {
					if (rollLeft) {
						roll = rotateSpeed * time_delta;
					}
					if (rollRight) {
						roll = -(rotateSpeed * time_delta);
					}
				}

				xoffset *= sensitivity * time_delta;
				yoffset *= sensitivity * time_delta;

				pitch = yoffset;

				if (speed > 0.0f) {
					front = glm::normalize(front * cos(glm::radians(pitch)) + up * sin(glm::radians(pitch)));
					up = glm::normalize(glm::cross(right, front));
				}

				right = glm::normalize(right * cos(glm::radians(roll)) + up * sin(glm::radians(roll)));
				up = glm::normalize(glm::cross(right, front));

				position += (speed * time_delta) * front;

				//build modelMatrix
				modelMatrix[0][0] = right.x;
				modelMatrix[0][1] = right.y;
				modelMatrix[0][2] = right.z;

				modelMatrix[1][0] = up.x;
				modelMatrix[1][1] = up.y;
				modelMatrix[1][2] = up.z;

				modelMatrix[2][0] = front.x;
				modelMatrix[2][1] = front.y;
				modelMatrix[2][2] = front.z;

				modelMatrix[3][0] = position.x;
				modelMatrix[3][1] = position.y;
				modelMatrix[3][2] = position.z;
			}

		private:
			float speed, rotateSpeed, sensitivity;
			float totalPitch, totalYaw;
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