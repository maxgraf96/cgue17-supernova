#pragma once

#include<glm/glm.hpp>
#include <glew.h>
#include <memory>
#include "glm/gtc/matrix_transform.hpp"


#include "../Shader.hpp"

/* Loosely based on http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=26 */

#define NUM_PARTICLE_ATTRIBUTES 6
#define MAX_PARTICLES_ON_SCENE 100000

#define PARTICLE_TYPE_GENERATOR 0
#define PARTICLE_TYPE_NORMAL 1

namespace supernova {
	namespace particles {
		class Particle 
		{
		public:
			glm::vec3 position;
			glm::vec3 velocity;
			glm::vec3 color;
			float lifeTime;
			float size;
			int type;
		};

		class ParticleTF
		{
		public:
			bool initializeParticleSystem();

			void renderParticles();
			void updateParticles(float timePassed);

			void setGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector,
				glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate, GLuint texture);

			void setMatrices(glm::mat4* inputProjection, glm::mat4* view, glm::vec3 inputUpVector);

			ParticleTF();

			virtual ~ParticleTF();

		private:
			bool initialized;

			GLuint transformFeedbackBuffer;

			GLuint particleBuffer[2];
			GLuint vao[2];

			GLuint query;
			GLuint texture;

			int curReadBuffer;
			int numParticles;

			glm::mat4 projection, view;
			glm::mat4 myVP;
			glm::vec3 quad1, quad2;

			float elapsedTime;
			float nextGenerationTime;

			glm::vec3 genPosition;
			glm::vec3 genVelocityMin, genVelocityRange;
			glm::vec3 genGravityVector;
			glm::vec3 genColor;

			float genLifeMin, genLifeRange;
			float genSize;

			int numToGenerate;

			std::unique_ptr<Shader> renderParticlesShader;
			std::unique_ptr<Shader> updateParticlesShader;
		};
	}
}