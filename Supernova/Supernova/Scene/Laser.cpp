#include "Laser.hpp"

using namespace supernova;
using namespace supernova::scene;

Laser::Laser() : Cube()
{
}

Laser::Laser(glm::mat4& matrix, Shader* shader, Material* material) :
	Cube(matrix, shader, material)
{
}


Laser::~Laser()
{
}

void Laser::update(glm::mat4& spaceshipMatrix, glm::vec3& front) {
	modelMatrix = glm::translate(glm::scale(spaceshipMatrix, glm::vec3(0.2f, 0.2f, 10.0f)), front);
}
