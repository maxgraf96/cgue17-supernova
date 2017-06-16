#pragma once

#include <iostream>

namespace supernova {
	namespace scene {
		enum class BoundingObjectType { AABB, Sphere };

		class BoundingSphere;
		class AABB;

		class BoundingObject
		{
		public:
			BoundingObject();
			BoundingObject(BoundingObjectType type);
			~BoundingObject();

			virtual bool collides(BoundingSphere* sphere) = 0;
			virtual bool collides(AABB* box) = 0;

			BoundingObjectType type;
		};
	}
}

