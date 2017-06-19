#include "Frustum.hpp"
#include "Scene\CollisionDetection\BoundingSphere.hpp"

using namespace supernova;

Frustum::Frustum()
{
}


Frustum::~Frustum()
{
}

void Frustum::setCameraParameters(float _nearDistance, float _farDistance, float _ratio, float _angle) {
	nearDistance = _nearDistance;
	farDistance = _farDistance;
	ratio = _ratio;
	angle = _angle;

	tang = (float)glm::tan(glm::radians(angle * 0.5f));
	nearHeight = nearDistance * tang;
	nearWidth = nearHeight * ratio;
	farHeight = farDistance * tang;
	farWidth = farHeight * ratio;
}

void Frustum::update(glm::vec3 &position, glm::vec3 &front, glm::vec3 &up) {
	vec3 dir, nearCenter, farCenter, x, y, z;

	//compute z-axis
	z = position - front;
	glm::normalize(z);

	//compute x-axis
	x = glm::cross(up, z);
	glm::normalize(x);

	//compute y-axis
	y = glm::cross(z, x);
	glm::normalize(y);

	//compute centers of near and far plane
	nearCenter = position - z * nearDistance;
	farCenter = position - z * farDistance;

	//compute 4 corners on near plane
	ntl = nearCenter + y * nearHeight - x * nearWidth;
	ntr = nearCenter + y * nearHeight + x * nearWidth;
	nbl = nearCenter - y * nearHeight - x * nearWidth;
	nbr = nearCenter - y * nearHeight + x * nearWidth;

	//compute 4 corners on far plane
	ftl = farCenter + y * farHeight - x * farWidth;
	ftr = farCenter + y * farHeight + x * farWidth;
	fbl = farCenter - y * farHeight - x * farWidth;
	fbr = farCenter - y * farHeight + x * farWidth;

	planes[TOP].generatePlane(ntr, ntl, ftl);
	planes[BOTTOM].generatePlane(nbl, nbr, fbr);
	planes[LEFT].generatePlane(ntl, nbl, fbl);
	planes[RIGHT].generatePlane(nbr, ntr, fbr);
	planes[NEARP].generatePlane(ntl, ntr, nbr);
	planes[FARP].generatePlane(ftr, ftl, fbl);
}

void Frustum::update(glm::mat4 &modelview, glm::mat4 &projection) {
	setFrustum(modelview * projection);
}

int Frustum::testSphere(scene::BoundingSphere* sphere) {
	float distance;
	int result = INSIDE;

	for (int i = 0; i < 6; i++) {
		distance = planes[i].calcualteDistanceToPoint(sphere->position);

		if (distance < -sphere->radius) {
			return OUTSIDE;
		}
		else if (distance < sphere->radius) {
			result = INTERSECT;
		}
	}

	return(result);
}

void Frustum::setFrustum(glm::mat4 &matrix) {
	planes[NEARP].generatePlane(
		matrix[2][0] + matrix[3][0],
		matrix[2][1] + matrix[3][1],
		matrix[2][2] + matrix[3][2],
		matrix[2][3] + matrix[3][3]
	);

	planes[FARP].generatePlane(
		-matrix[2][0] + matrix[3][0],
		-matrix[2][1] + matrix[3][1],
		-matrix[2][2] + matrix[3][2],
		-matrix[2][3] + matrix[3][3]
	);

	planes[BOTTOM].generatePlane(
		matrix[1][0] + matrix[3][0],
		matrix[1][1] + matrix[3][1],
		matrix[1][2] + matrix[3][2],
		matrix[1][3] + matrix[3][3]
	);

	planes[TOP].generatePlane(
		-matrix[1][0] + matrix[3][0],
		-matrix[1][1] + matrix[3][1],
		-matrix[1][2] + matrix[3][2],
		-matrix[1][3] + matrix[3][3]
	);

	planes[LEFT].generatePlane(
		matrix[0][0] + matrix[3][0],
		matrix[0][1] + matrix[3][1],
		matrix[0][2] + matrix[3][2],
		matrix[0][3] + matrix[3][3]
	);

	planes[RIGHT].generatePlane(
		-matrix[0][0] + matrix[3][0],
		-matrix[0][1] + matrix[3][1],
		-matrix[0][2] + matrix[3][2],
		-matrix[0][3] + matrix[3][3]
	);
}
