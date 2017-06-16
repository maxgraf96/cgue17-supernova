#include "BoundingObject.hpp"

using namespace supernova;
using namespace supernova::scene;

BoundingObject::BoundingObject() {

}

BoundingObject::BoundingObject(BoundingObjectType _type) : type(_type)
{
}


BoundingObject::~BoundingObject()
{
}

/*
bool BoundingObject::collides(BoundingSphere* other)
{
	std::cerr << "Trying to detect collision with BoundingObject abstract class!" << std::endl;
	return false;
}
*/
