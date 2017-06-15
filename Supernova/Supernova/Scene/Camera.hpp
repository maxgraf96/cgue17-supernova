#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <memory>

namespace supernova {
	namespace scene {
		class Camera {
		public:
			Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right);

			virtual ~Camera();
			
			virtual void update(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 right);
			glm::mat4 viewMatrix;
			glm::vec3& getPosition(){
				return position;
			}
			glm::vec3& getFront() {
				return front;
			}
			glm::vec3& getUp() {
				return up;
			}


		private:
			//Camera Attributes
			glm::vec3 position, front, up, right;
		};
	}
}