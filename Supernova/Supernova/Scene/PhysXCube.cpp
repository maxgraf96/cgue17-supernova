#include "PhysXCube.hpp"

using namespace supernova;
using namespace supernova::scene;

PhysXCube::PhysXCube() : Cube()
{
}

PhysXCube::PhysXCube(glm::mat4& matrix, physx::PxRigidActor* _actor, Shader* shader, Material* material)
	: Cube(matrix, shader, material), actor(_actor){
}

PhysXCube::~PhysXCube()
{
	Cube::~Cube();
}

void PhysXCube::draw() {
	Cube::draw();
}