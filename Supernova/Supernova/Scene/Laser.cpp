#include "Laser.hpp"

using namespace supernova;
using namespace supernova::scene;

Laser::Laser() : Model()
{
}

Laser::Laser(glm::mat4& matrix, string const &path) :
	Model(matrix, path), shooting(false)
{
	vec3 color = vec3(1.0f, 1.0f, 1.0f);
	float shininess = 2.0f;

	std::unique_ptr<Material> material = std::make_unique<Material>(color, color, shininess);
	for (GLuint i = 0; i < this->meshes.size(); i++) {
		this->meshes[i].setNoTextureMaterial(material.get());
	}

	boundingBox = AABB(meshes, modelMatrix);
}

void Laser::update(float time_delta, int pressed, glm::mat4& spaceshipMatrix, glm::vec3 cameraFront) {
	modelMatrix = glm::scale(glm::translate(spaceshipMatrix, cameraFront * 1.0f), glm::vec3(0.2, 0.2, 100.0));
	//modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0.2));
}

void Laser::update(float time_delta, int pressed){}

Laser::~Laser()
{
}

void Laser::setShooting(bool _shooting) {
	shooting = _shooting;
}

bool Laser::getShooting() {
	return shooting;
}
