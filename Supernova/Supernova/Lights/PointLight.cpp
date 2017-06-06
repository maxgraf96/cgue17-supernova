#pragma once

#include <glew.h>
#include <glm\glm.hpp>

#include<memory>
#include<map>

#include "../Lights/Light.cpp"

namespace supernova {
	using namespace glm;
	namespace scene {
		class PointLight : public Light 
		{
		public:
			PointLight(vec3 _ambient, vec3 _diffuse, vec3 _specular, vec3 _position,
				float _constant, float _linear, float _quadratic) :
				Light(_position, _ambient, _diffuse, _specular), 
				constant(_constant), linear(_linear), quadratic(_quadratic) {};

			float& getConstant() {
				return constant;
			}
			float& getLinear() {
				return linear;
			}
			float& getQuadratic() {
				return quadratic;
			}

		private:
			float constant;
			float linear;
			float quadratic;
		};
	}
}