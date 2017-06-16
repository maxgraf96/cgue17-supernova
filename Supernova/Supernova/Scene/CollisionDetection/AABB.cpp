#include "AABB.hpp"

using namespace supernova;
using namespace supernova::scene;

AABB::AABB() {

}

AABB::AABB(vector<Mesh>& meshes, glm::mat4& modelMatrix) : BoundingObject(BoundingObjectType::AABB)
{
	calculateAABB(meshes, modelMatrix);
}


AABB::~AABB()
{
}

void AABB::calculateAABB(vector<Mesh>& _meshes, glm::mat4& _modelMatrix) {
	bool first = true;

	int meshsize = _meshes.size();
	for (int i = 0; i < meshsize; i++)
	{
		vector<Vertex>& vertices = _meshes[i].vertices;
		int vertexsize = vertices.size();
		for (int j = 0; j < vertexsize; j++)
		{
			glm::vec4 pos = vec4(vertices[j].position, 1.0);
			glm::vec4 posWorldSpace = _modelMatrix * pos;
			glm::vec3 position = glm::vec3(posWorldSpace.x / posWorldSpace.w, posWorldSpace.y / posWorldSpace.w, posWorldSpace.z / posWorldSpace.w);

			if (first) {
				min.x = position.x;
				min.y = position.y;
				min.z = position.z;

				max.x = position.x;
				max.y = position.y;
				max.z = position.z;

				first = false;
			}
			else {
				if (position.x < min.x) min.x = position.x;
				if (position.y < min.y) min.y = position.y;
				if (position.z < min.z) min.z = position.z;
				
				if (position.x > max.x) max.x = position.x;
				if (position.y > max.y) max.y = position.y;
				if (position.z > max.z) max.z = position.z;
			}
		}
	}
}

bool AABB::collides(BoundingSphere* sphere) {
	//Box vs. Sphere
	float x = glm::max(min.x, glm::min(sphere->position.x, max.x));
	float y = glm::max(min.y, glm::min(sphere->position.y, max.y));
	float z = glm::max(min.z, glm::min(sphere->position.z, max.z));

	float distance = glm::sqrt((x - sphere->position.x) * (x - sphere->position.x) +
								(y - sphere->position.y) * (y - sphere->position.y) +
								(z - sphere->position.z) * (z - sphere->position.z));

	return (distance < sphere->radius);
}

bool AABB::collides(AABB* box) {
	//Box vs. Box
	return ((min.x <= box->min.x && max.x >= box->max.x) &&
		(min.y <= box->min.y && max.y >= box->max.y) &&
		(min.z <= box->min.z && max.z >= box->max.z));
}
