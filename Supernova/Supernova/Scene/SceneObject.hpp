#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "../Particles/ExtPTF.hpp"

namespace supernova {
	namespace scene {
		class SceneObject {
		public:
			SceneObject(glm::mat4& modelMatrix);

			virtual ~SceneObject();

			virtual void draw() = 0;
			virtual void update(float time_delta, int pressed) = 0;

			glm::mat4 modelMatrix;

			ExtPTF particleSystem;

			glm::vec3 getPosition();
			glm::vec3 getPositionFromModelMatrix(glm:: mat4 &_modelMatrix);

		};
	}
}
#define SCENE_OBJECT