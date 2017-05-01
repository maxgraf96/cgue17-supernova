#pragma once
#include <vector>
#include <glm/glm.hpp>

namespace supernova {
	namespace scene {
		class SceneObject {
		public:
			SceneObject(glm::mat4& modelMatrix);

			virtual ~SceneObject();

			virtual void draw() = 0;
			virtual void update(float time_delta, int pressed) = 0;

			glm::mat4 modelMatrix;
		};
	}
}
#define SCENE_OBJECT