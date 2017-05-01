#pragma once
#include <glm\glm.hpp>

namespace supernova {
	using namespace glm;
	namespace scene {
		class Material
		{
		public:
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
			float& getShininess() {
				return shininess;
			}
			void setShininess(const float& shininess) {
				this->shininess = shininess;
			}
		private:
			vec3 ambient;
			vec3 diffuse;
			vec3 specular;
			float shininess;
		};
	}
}