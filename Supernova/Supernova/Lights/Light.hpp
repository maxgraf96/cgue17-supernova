#pragma once

#include <glew.h>
#include <glm\glm.hpp>

#include<memory>
#include<map>

namespace supernova {
	using namespace glm;
	namespace scene {
		/* Base class */
		class Light
		{
		public:
			Light(vec3 _ambient, vec3 _diffuse, vec3 _specular, vec3 &_position) :
				position(_position), ambient(_ambient), diffuse(_diffuse), specular(_specular) {};
			Light() : position(glm::vec3(0)), ambient(glm::vec3(0)), diffuse(glm::vec3(0)), specular(glm::vec3(0)) {};
			vec3& getAmbient() {
				return ambient;
			}
			void setAmbient(vec3& ambient) {
				this->ambient = ambient;
			}
			vec3& getDiffuse() {
				return diffuse;
			}
			void setDiffuse(vec3& diffuse) {
				this->diffuse = diffuse;
			}
			vec3& getSpecular() {
				return specular;
			}
			void setSpecular(vec3& specular) {
				this->specular = specular;
			}
			vec3& getPosition() {
				return position;
			}
			void setShininess(vec3& position) {
				this->position = position;
			}
			bool isInitialized() {
				return initialized;
			}
			void setInitialized(bool _initialized) {
				initialized = _initialized;
			}
		private:
			vec3 position;
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
			bool initialized = false;
		};

		/* Directional light */
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


		/* Point light */
		class PointLight : public Light
		{
		public:
			PointLight(vec3 _ambient, vec3 _diffuse, vec3 _specular, vec3 _position,
				float _constant, float _linear, float _quadratic) :
				Light(_position, _ambient, _diffuse, _specular),
				constant(_constant), linear(_linear), quadratic(_quadratic) {};
			PointLight() : Light(glm::vec3(0), glm::vec3(0), glm::vec3(0), glm::vec3(0)),
				constant(0), linear(0), quadratic(0) {};

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