#pragma once

#include <glew.h>
#include "Cube.hpp"
#include "../Shader.hpp"
#include "../Materials/Material.hpp"
#include "../Materials/Metal.cpp"

#ifdef SCENE_OBJECT

#define CUBE_VERTEX_COUNT 24
#define CUBE_INDEX_COUNT 36

namespace supernova {
	namespace scene {
		class MovingCube : public Cube
		{
		public:
			MovingCube();
			MovingCube(glm::mat4& matrix, Shader* shader, Material* material, float zOffset);
			virtual ~MovingCube();

			virtual void update(float time_delta, int pressed);
			virtual void draw();
		};
	}
}
#endif // SCENE_OBJECT