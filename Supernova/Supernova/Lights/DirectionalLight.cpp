#pragma once

#include <glew.h>
#include <glm\glm.hpp>

#include<memory>
#include<map>

#include "../Lights/Light.cpp"

namespace supernova {
	using namespace glm;
	namespace scene {
		class DirectionalLight : public Light
		{
		public:
			DirectionalLight(vec3 _ambient, vec3 _diffuse, vec3 _specular, vec3 _direction) :
				Light(glm::vec3(), _ambient, _diffuse, _specular),
				direction(_direction) {};

			vec3& getDirection() {
				return direction;
			}

		private:
			vec3 direction;
		};
	}
}