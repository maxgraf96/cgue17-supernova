#pragma once

#include <glew.h>
#include "SceneObject.hpp"
#include "../Shader.hpp"
#include "../Materials/Material.hpp"

#define SPACESHIP_VERTEX_COUNT 18
#define SPACESHIP_INDEX_COUNT 24

namespace supernova {
	namespace scene {
		class Spaceship : public SceneObject
		{
		public:
			Spaceship();
			Spaceship(glm::mat4& matrix, Shader* shader, Material* material);
			virtual ~Spaceship();

			virtual void update(float time_delta, int pressed);
			virtual void update(float time_delta, int pressed, bool forward, bool backward, bool rollLeft, bool rollRight, GLfloat xoffset, GLfloat yoffset);
			virtual void draw();

		private:
			GLuint vao;
			GLuint positionBuffer, normalsBuffer, indexBuffer;
			Shader* shader;
			Material* material;
			glm::vec3 front, up, right;
			float totalYaw, totalPitch;

			static const float positions[SPACESHIP_VERTEX_COUNT * 3];
			static const unsigned int indices[SPACESHIP_INDEX_COUNT];
			static const float normals[SPACESHIP_VERTEX_COUNT * 3];
		};
	}
}

