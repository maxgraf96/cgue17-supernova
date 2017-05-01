#pragma once

#include <glew.h>
#include "SceneObject.hpp"
#include "../Shader.hpp"

#ifdef SCENE_OBJECT

#define LIGHTCUBE_VERTEX_COUNT 24
#define LIGHTCUBE_INDEX_COUNT 36

namespace supernova {
	namespace scene {
		class LightCube : public SceneObject
		{
		public:
			LightCube();
			LightCube(glm::mat4& matrix, Shader* shader);
			virtual ~LightCube();

			virtual void update(float time_delta, int pressed);
			virtual void draw();

		private:
			GLuint vao;
			GLuint positionBuffer, normalsBuffer, indexBuffer;
			Shader* shader;

			static const float positions[LIGHTCUBE_VERTEX_COUNT * 3];
			static const unsigned int indices[LIGHTCUBE_INDEX_COUNT];
			static const float normals[LIGHTCUBE_VERTEX_COUNT * 3];
		};
	}
}
#endif // SCENE_OBJECT