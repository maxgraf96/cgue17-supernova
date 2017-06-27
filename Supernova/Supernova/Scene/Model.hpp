#pragma once
#include <vector>
#include<glm/glm.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "SceneObject.hpp"
#include "Mesh.hpp"
#include "../Textures/TextureLoader.hpp"
#include "CollisionDetection\AABB.hpp"
#include "CollisionDetection\BoundingSphere.hpp"


namespace supernova {
	namespace scene {
		class Model : public SceneObject {
		public:
			Model();
			Model(glm::mat4& matrix, string const &path);
			virtual ~Model();
			virtual void update(float time_delta, int pressed);
			virtual void draw(Shader* shader);
			virtual void translate(glm::vec3 direction);
			// Need bc extending SceneObject - does nothing!
			virtual void draw();
			vector<Mesh> meshes;
		private:
			TextureLoader textureLoader;
			string directory;
			vector<Texture> alreadyLoadedTextures;

			void loadModel(string const &path);
			void processNode(aiNode* node, const aiScene* scene);
			Mesh processMesh(aiMesh* mesh, const aiScene* scene);
			vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
		};

		/* --------------------------- Radar --------------------------- */
		class Radar
			: public supernova::scene::Model {

		public:
			BoundingSphere boundingSphere;

			Radar(glm::mat4& matrix, string const &path) : Model(matrix, path) {
				// Create radar material
				vec3 color = vec3(0.92f, 0.94f, 0.95f);
				float shininess = 16;

				std::unique_ptr<Material> radarMaterial = std::make_unique<Material>(color, color, shininess);
				for (GLuint i = 0; i < this->meshes.size(); i++) {
					this->meshes[i].setNoTextureMaterial(*radarMaterial.get());
				}
			}
			Radar() : Model(glm::mat4(1.0f), "") {}
			void update(float time_delta, int pressed, mat4& spaceshipMatrix, bool rotationDirection, bool retracting, bool retracted) {
				
				currentDeg += 3.0f * time_delta;
				if (currentDeg > 360.0f)
					currentDeg = 0.0f;

				modelMatrix = glm::rotate(
					glm::translate(spaceshipMatrix, vec3(0, 0.2f, -5.5)),
					glm::radians(90.0f), glm::vec3(0, 1, 0));

				// Continuous rotation
				if (!rotationDirection)
					modelMatrix = glm::rotate(modelMatrix, currentDeg, glm::vec3(0, 1, 0));
				else if (rotationDirection)
					modelMatrix = glm::rotate(modelMatrix, -currentDeg, glm::vec3(0, 1, 0));
				else
					modelMatrix = glm::rotate(modelMatrix, -currentDeg, glm::vec3(0, 1, 0));
				
				/*if (!retracting) {
					currentDeg += 3.0f * time_delta;
					if (currentDeg > 360.0f)
						currentDeg = 0.0f;

					vec3 spaceshipPosition;
					modelMatrix = glm::rotate(
						glm::translate(spaceshipMatrix, vec3(0, 0.2f, -5.5)),
						glm::radians(90.0f), glm::vec3(0, 1, 0));
				}
				
				// Check if retracting
				if (retracting) {
					// rotate until aligned with spaceship
					if (currentDeg < 1) {
						currentDeg += 3.0f * time_delta;
						if (currentDeg > 360.0f)
							currentDeg = 0.0f;

						if (!rotationDirection)
							modelMatrix = glm::rotate(modelMatrix, currentDeg, glm::vec3(0, 1, 0));
						else if (rotationDirection)
							modelMatrix = glm::rotate(modelMatrix, -currentDeg, glm::vec3(0, 1, 0));
					}
					// aligned -> sink into spaceship
					else {
						if (!retracted && !sunkenIn) {
							// sink
							if (modelMatrix[3][2] < 1.0f) {
								modelMatrix = glm::translate(modelMatrix, glm::vec3(0, -1, 0) * time_delta);
							}
							else {
								sunkenIn = true;
								int a = 2;
							}
						}
						else {

						}
					}
				}
				else {
					// Continuous rotation
					if (!rotationDirection)
						modelMatrix = glm::rotate(modelMatrix, currentDeg, glm::vec3(0, 1, 0));
					else if (rotationDirection)
						modelMatrix = glm::rotate(modelMatrix, -currentDeg, glm::vec3(0, 1, 0));
					else
						modelMatrix = glm::rotate(modelMatrix, -currentDeg, glm::vec3(0, 1, 0));
				}*/
			}
			// Need
			void update(float time_delta, int pressed) {}
		private:
			float currentDeg = 0.0f;
			bool sunkenIn = false;
		};

		/* --------------------------- Spaceship --------------------------- */
		class Spaceship
			: public supernova::scene::Model {

		public:
			vec3 front, up, right;
			AABB boundingBox;
			BoundingSphere boundingSphere;

			Spaceship(glm::mat4& matrix, string const &path) : Model(matrix, path),
			speed(0.0f), rotateSpeed(90.0f), sensitivity(5.0f) {
				front = glm::vec3(0.0f, 0.0f, 1.0f);
				up = glm::vec3(0.0f, -1.0f, 0.0f);
				right = glm::cross(front, up);

				boundingBox = AABB(meshes, modelMatrix);
				boundingSphere = BoundingSphere(meshes, modelMatrix);
			};

			void update(float time_delta, int pressed) {}

			//TODO: give all Bounding Spheres to test against!
			void update(bool forward, bool backward, bool rollLeft, bool rollRight, float xoffset, float yoffset,
				float time_delta, vector<BoundingSphere*> obstacles) {
 				glm::vec3 position = getPosition();
				glm::vec3 oldPosition = position;

				float roll = 0.0f;
				float pitch = 0.0f;

				if (forward) {
					if (speed < 70.0f) {
						speed += 15.0f * time_delta;
					}
				}
				if (backward) {
					if (speed > 0.0f) {
						speed -= 15.0f * time_delta;
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

				//test if collision occurs:
				boundingSphere.setPosition(getPosition());

				int size = obstacles.size();
				for (int i = 0; i < size; i++)
				{
					if (boundingSphere.collides(obstacles[i])) {
						std::cout << "Collision detected!" << std::endl;
						modelMatrix[3][0] = oldPosition.x;
						modelMatrix[3][1] = oldPosition.y;
						modelMatrix[3][2] = oldPosition.z;
						speed = 0.001;
					}
				}
			}

		private:
			float speed, rotateSpeed, sensitivity;
			float totalPitch, totalYaw;			
		};

		/* --------------------------- Sun --------------------------- */
		class Sun
			: public supernova::scene::Model {

		public:
			BoundingSphere boundingSphere;

			Sun(glm::mat4& matrix, string const &path) : Model(matrix, path) {
				vec3 sunPosition = vec3(0, 0, -1500);
				modelMatrix = glm::translate(modelMatrix, sunPosition);
				modelMatrix = glm::scale(modelMatrix, vec3(300.0f));
				// Create Sun material - it's white
				vec3 color = vec3(1.0f);
				float shininess = 0;

				std::unique_ptr<Material> sunMaterial = std::make_unique<Material>(color, color, shininess);
				for (GLuint i = 0; i < this->meshes.size(); i++) {
					this->meshes[i].setNoTextureMaterial(*sunMaterial.get());
				}

				boundingSphere = BoundingSphere(meshes, modelMatrix);
			}
			void update(float time_delta, int pressed) override {
				// ...
			}
		};

		/* ------------------ Asteroide -------------------------- */
		class Asteroid
			: public supernova::scene::Model {

		public:
			BoundingSphere boundingSphere;

			Asteroid(glm::mat4& matrix, string const & path) : Model(matrix, path), destroyed(false), damage(0) {
				//Give material (only as long as it is not an asteroid)
				// Generate more or less random color asteroids
				glm::vec3 blueGreyish = glm::vec3(0.153f, 0.18f, 0.23f);
				glm::vec3 darkerGrey = glm::vec3(0.188f, 0.20f, 0.224f);
				glm::vec3 lighterGrey = glm::vec3(0.58f, 0.58f, 0.58f);
				glm::vec3 exoticYellow = glm::vec3(100, 0.914f, 0);
				glm::vec3 exoticRed = glm::vec3(0.506f, 0.024f, 0.024f);
				glm::vec3 color;
				int colorSeed = 0 + rand() % 10;
				if (colorSeed == 2)
					color = exoticRed;
				else if (colorSeed == 9)
					color = exoticYellow;
				else {
					colorSeed = 0 + rand() % 10;
					if (colorSeed < 3)
						color = blueGreyish;
					else if (colorSeed >= 3 && colorSeed < 7)
						color = darkerGrey;
					else
						color = lighterGrey;
				}

				float shininess = 32.0f;

				std::unique_ptr<Material> material = std::make_unique<Material>(color, vec3(1.0f), shininess);
				for (GLuint i = 0; i < this->meshes.size(); i++) {
					this->meshes[i].setNoTextureMaterial(*material.get());
				}

				boundingSphere = BoundingSphere(meshes, modelMatrix);
			}

			void detectHit(AABB* laser, float time_delta) {
				boundingSphere.setPosition(getPosition());

				if (boundingSphere.collides(laser)) {
					damage += 100 * time_delta;
				}

				if (damage >= 100) {
					destroyed = true;
				}
			}

			void update(float time_delta, int pressed) override {
				// Make smaller
				//modelMatrix = glm::scale(modelMatrix, glm::vec3(0.9f));
			}

			bool getDestroyed() {
				return destroyed;
			}

		private:
			bool destroyed;
			int damage;
		};
	}
}