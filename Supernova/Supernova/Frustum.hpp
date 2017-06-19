#pragma once

#include <glm/glm.hpp>

#include "Scene/CollisionDetection/BoundingSphere.hpp"

//based on the tutorial from "Lighthouse3D" (http://www.lighthouse3d.com/tutorials/view-frustum-culling/)
namespace supernova {
	class Plane {
	public:
		Plane() {
		}

		~Plane() {
		}

		void generatePlane(float a, float b, float c, float d) {
			normal.x = a;
			normal.y = b;
			normal.z = c;

			float l = normal.length();

			glm::normalize(normal);

			distance = d / l;
		}

		void generatePlane(glm::vec3 normal, glm::vec3 point) {
			this->normal = normal;
			this->point = point;
			distance = -(this->normal.x * point.x + this->normal.y * point.y + this->normal.z * point.z);
		}

		void generatePlane(glm::vec3 v1, glm::vec3 v2, glm::vec3 v3) {
			glm::vec3 aux1, aux2;

			aux1 = v1 - v2;
			aux2 = v3 - v2;

			normal = glm::cross(aux2, aux1);
			normal = glm::normalize(normal);

			point = v2;

			distance = -(this->normal.x * point.x + this->normal.y * point.y + this->normal.z * point.z);
		}

		glm::vec3 getPoint() {
			return point;
		}

		glm::vec3 getNormal() {
			return normal;
		}

		float getDistance() {
			return distance;
		}

		float calcualteDistanceToPoint(glm::vec3 point) {
			return normal.x * point.x + normal.y * point.y + normal.z * point.z + distance;
		}

	private:
		glm::vec3 point;
		glm::vec3 normal;
		float distance;
	};

	class Frustum
	{

	private:
		enum {
			TOP = 0, BOTTOM, LEFT, RIGHT, NEARP, FARP
		};

	public:

		static enum { OUTSIDE, INTERSECT, INSIDE };
		Plane planes[6];
		glm::vec3 ntl, ntr, nbl, nbr, ftl, ftr, fbl, fbr;
		float nearDistance, farDistance, ratio, angle, tang;
		float nearWidth, nearHeight, farWidth, farHeight;

		Frustum();
		~Frustum();

		void setCameraParameters(float nearDistance, float farDistance, float ratio, float angle);
		void update(glm::vec3 &position, glm::vec3 &front, glm::vec3 &up);
		void update(glm::mat4 &modelview, glm::mat4 &projection);

		int testSphere(scene::BoundingSphere* sphere);

	private:
		void setFrustum(glm::mat4 &matrix);
	};
}


