#include "CollisionDetector.hpp"

using namespace supernova;
using namespace supernova::scene;

CollisionDetector::CollisionDetector() : physx::PxSimulationEventCallback()
{
}


CollisionDetector::~CollisionDetector()
{
	physx::PxSimulationEventCallback::~PxSimulationEventCallback();
}

void CollisionDetector::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {

}
