#pragma once

#include <glew.h>
#include <glm\glm.hpp>

#include<memory>
#include<map>

/*________________________________________________



Use later to replace light cube with abstract light objects!


__________________________________________________*/


namespace supernova {
	using namespace glm;
	namespace scene {
		class Light
		{
		public:
			Light(vec3 _ambient, vec3 _diffuse, vec3 _specular, vec3 _position) :
				position(_position), ambient(_ambient), diffuse(_diffuse), specular(_specular) {};
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
		private:
			vec3 position;
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
		};
	}
}