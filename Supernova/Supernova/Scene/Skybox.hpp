#pragma once

#include <glew.h>
#include "SceneObject.hpp"
#include "../Shader.hpp"

#ifdef SCENE_OBJECT

#define SKYBOX_VERTEX_COUNT 24
#define SKYBOX_INDEX_COUNT 36

namespace supernova {
	namespace scene {
		class Skybox : public SceneObject
		{
		public:
			Skybox();
			Skybox(glm::mat4& matrix, Shader* shader, GLuint cubeMapTexture);
			virtual ~Skybox();

			virtual void update(float time_delta, int pressed);
			virtual void draw();

		private:
			GLuint vao;
			GLuint positionBuffer, normalsBuffer, indexBuffer;
			GLuint cubeMapTexture;
			Shader* shader;

			static const float positions[SKYBOX_VERTEX_COUNT * 3];
			static const unsigned int indices[SKYBOX_INDEX_COUNT];
			static const float normals[SKYBOX_VERTEX_COUNT * 3];
		};
	}
}
#endif // SCENE_OBJECT