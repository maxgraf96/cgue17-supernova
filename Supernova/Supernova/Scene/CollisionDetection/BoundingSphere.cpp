#include "BoundingSphere.hpp"

using namespace supernova;
using namespace supernova::scene;

BoundingSphere::BoundingSphere() : BoundingObject(BoundingObjectType::Sphere)
{
}

BoundingSphere::BoundingSphere(vector<Mesh>& meshes, glm::mat4& modelMatrix) : BoundingObject(BoundingObjectType::Sphere), radius(0.0f), position(glm::vec3(0.0f, 0.0f, 0.0f))
{
	calculateBoundingSphere(meshes, modelMatrix);
}

BoundingSphere::~BoundingSphere()
{
}

void BoundingSphere::calculateBoundingSphere(vector<Mesh>& _meshes, glm::mat4& _modelMatrix) {
	bool first = true;

	int meshsize = _meshes.size();
	for (int i = 0; i < meshsize; i++)
	{
		vector<Vertex>& vertices = _meshes[i].vertices;
		int vertexsize = vertices.size();
		for (int j = 0; j < vertexsize; j++)
		{
			glm::vec3 position = vertices[j].position;
			glm::vec4 realPosition = (glm::vec4(position, 1.0f) * _modelMatrix);
			position = glm::vec3(realPosition.x, realPosition.y, realPosition.z);

			if (first) {
				radius = glm::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
			}

			float r = glm::sqrt(position.x * position.x + position.y * position.y + position.z * position.z);
			if (radius < r) radius = r;
		}
	}
}

bool BoundingSphere::collides(BoundingSphere* sphere) {
	//Sphere vs. Sphere
	float distance = glm::sqrt((position.x - sphere->position.x) * (position.x - sphere->position.x) +
								(position.y - sphere->position.y) * (position.y - sphere->position.y) +
								(position.z - sphere->position.z) * (position.z - sphere->position.z));

	return (distance < (radius + sphere->radius));
}

bool BoundingSphere::collides(AABB* box) {
	//Sphere vs. Box
	float x = glm::max(box->min.x, glm::min(position.x, box->max.x));
	float y = glm::max(box->min.y, glm::min(position.y, box->max.y));
	float z = glm::max(box->min.z, glm::min(position.z, box->max.z));

	float distance = glm::sqrt((x - position.x) * (x - position.x) +
		(y - position.y) * (y - position.y) +
		(z - position.z) * (z - position.z));

	return (distance < radius);
}

void BoundingSphere::setPosition(glm::vec3 _position) {
	position = _position;
}