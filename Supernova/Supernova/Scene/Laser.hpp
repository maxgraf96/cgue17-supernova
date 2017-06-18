#pragma once

#include "Cube.hpp"

namespace supernova {
	namespace scene {
		class Laser : public Cube
		{
		public:
			Laser();
			Laser(glm::mat4& matrix, Shader* shader, Material* material);
			~Laser();

			void update(glm::mat4& spaceshipMatrix, glm::vec3& front);
		};
	}
}
