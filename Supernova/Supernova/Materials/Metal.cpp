#include "Material.hpp"

using namespace supernova;
using namespace supernova::scene;

class Metal : public Material {
public:
	Metal::Metal(vec3 color) 
		: Material(color, vec3(0.774597f, 0.774597f, 0.774597f), 76.8f) {
		Material::setAmbient(vec3(color.x, color.y, color.z));
		Material::setDiffuse(vec3(color.x, color.y, color.z));
	}
};