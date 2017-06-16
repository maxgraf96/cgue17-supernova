#pragma once

#include <glm\glm.hpp>
#include <vector>

#include "BoundingObject.hpp"
#include "../Mesh.hpp"
#include "AABB.hpp"

namespace supernova {
	namespace scene {
		class BoundingSphere : public BoundingObject
		{
		public:
			BoundingSphere();
			BoundingSphere(vector<Mesh>& meshes);
			~BoundingSphere();

			glm::vec3 position;
			float radius;

			void calculateBoundingSphere(vector<Mesh>& meshes);
			bool collides(BoundingSphere* sphere) override;
			bool collides(AABB* box) override;

			void setPosition(glm::vec3 position);
		};
	}
}

