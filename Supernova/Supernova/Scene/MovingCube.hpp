#pragma once

#include <glew.h>
#include "Cube.hpp"
#include "../Shader.hpp"
#include "../Materials/Material.hpp"
#include "../Materials/Metal.cpp"
#include "Camera.hpp"

#ifdef SCENE_OBJECT

#define CUBE_VERTEX_COUNT 24
#define CUBE_INDEX_COUNT 36

namespace supernova {
	namespace scene {
		class MovingCube : public Cube
		{
		public:
			MovingCube();
			MovingCube(glm::mat4& matrix, Shader* shader, Camera* _camera, Material* material, float zOffset);
			virtual ~MovingCube();

			virtual void update(float time_delta, int pressed);
			virtual void draw();
		private:
			float zOffset;
			Camera* camera;
		};
	}
}
#endif // SCENE_OBJECT