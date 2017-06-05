#pragma once

#include "Cube.hpp"
#include "Camera.hpp"
#include <physx-3.4\PxPhysicsAPI.h>
#include <physx-3.4\extensions\PxSimpleFactory.h>

namespace supernova {
	namespace scene {
		class PhysXCube : public Cube
		{
		public:
			PhysXCube();
			PhysXCube(glm::mat4& matrix, physx::PxRigidActor* actor, Shader* shader, Material* material);
			virtual ~PhysXCube();

			virtual void draw();

			physx::PxRigidActor* actor;
		};
	}
}

