#pragma once
#include <vector>
#include <glm\glm.hpp>

#include "BoundingObject.hpp"
#include "BoundingSphere.hpp"
#include "../Mesh.hpp"

namespace supernova {
	namespace scene {
		class AABB : public BoundingObject
		{
		public:
			AABB();
			AABB(vector<Mesh>& meshes, glm::mat4& modelMatrix);
			~AABB();

			void calculateAABB(vector<Mesh>& meshes, glm::mat4& modelMatrix);
			bool collides(BoundingSphere* sphere) override;
			bool collides(AABB* box) override;

			glm::vec3 min;
			glm::vec3 max;
		};
	}
}

