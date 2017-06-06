#pragma once

#include <physx-3.4\PxPhysicsAPI.h>
#include <physx-3.4\extensions\PxSimpleFactory.h>

namespace supernova {
	namespace scene {
		class CollisionDetector : public physx::PxSimulationEventCallback
		{
		public:
			CollisionDetector();
			~CollisionDetector();
			virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		};
	}
}

