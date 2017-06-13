#include "ParticleTF.hpp"
#include <iostream>


using namespace supernova;
using namespace supernova::particles;

ParticleTF::ParticleTF()
	: initialized(false), curReadBuffer(0)
{
}


ParticleTF::~ParticleTF()
{
}

/* Generate random float */
float grandf(float min, float add)
{
	float random = float(rand() % (RAND_MAX + 1)) / float(RAND_MAX);
	return min + add*random;
}

bool ParticleTF::initializeParticleSystem() {
	if(initialized)return false;

	const char* varyings[NUM_PARTICLE_ATTRIBUTES] =
	{
		"positionOut",
		"velocityOut",
		"colorOut",
		"lifeTimeOut",
		"sizeOut",
		"typeOut",
	};

	// Updating program
	updateParticlesShader = std::make_unique<Shader>("Shader/particleUpdate.vert", "Shader/particleUpdate.geom", false);
	for (int i = 0; i < NUM_PARTICLE_ATTRIBUTES; i++) {
		glTransformFeedbackVaryings(updateParticlesShader->programHandle, 6, varyings, GL_INTERLEAVED_ATTRIBS);
	}
	updateParticlesShader->link();

	// Rendering program
	std::string geom = "Shader/particleRender.geom";
	renderParticlesShader = std::make_unique<Shader>("Shader/particleRender.vert", "Shader/particleRender.frag", geom);
	renderParticlesShader->link();

	glGenTransformFeedbacks(1, &transformFeedbackBuffer);
	glGenQueries(1, &query);

	glGenBuffers(2, particleBuffer);
	glGenVertexArrays(2, vao);

	Particle partInitialization;
	partInitialization.type = PARTICLE_TYPE_GENERATOR;

	for(int i = 0; i < 2; i++)
	{
		glBindVertexArray(vao[i]);
		glBindBuffer(GL_ARRAY_BUFFER, particleBuffer[i]);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Particle)*MAX_PARTICLES_ON_SCENE, NULL, GL_DYNAMIC_DRAW);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Particle), &partInitialization);

		for (int i = 0; i < NUM_PARTICLE_ATTRIBUTES; i++) {
			glEnableVertexAttribArray(i);
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)0); // Position
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)12); // Velocity
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)24); // Color
		glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)36); // Lifetime
		glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)40); // Size
		glVertexAttribPointer(5, 1, GL_INT, GL_FALSE, sizeof(Particle), (const GLvoid*)44); // Type
	}

	curReadBuffer = 0;
	numParticles = 1;

	initialized = true;

	return true;
}

void ParticleTF::updateParticles(float timePassed) {
	if(!initialized)return;

	updateParticlesShader->useShader();

	updateParticlesShader->setUniform("myVPVert", myVP);

	updateParticlesShader->setUniform("timePassed", timePassed);
	updateParticlesShader->setUniform("genPosition", genPosition);
	updateParticlesShader->setUniform("genVelocityMin", genVelocityMin);
	updateParticlesShader->setUniform("genVelocityRange", genVelocityRange);
	updateParticlesShader->setUniform("genColor", genColor);
	updateParticlesShader->setUniform("genGravityVector", genGravityVector);

	updateParticlesShader->setUniform("genLifeMin", genLifeMin);
	updateParticlesShader->setUniform("genLifeRange", genLifeRange);

	updateParticlesShader->setUniform("genSize", genSize);
	updateParticlesShader->setUniform("numToGenerate", 0);

	elapsedTime += timePassed;

	if (elapsedTime > nextGenerationTime)
	{
		updateParticlesShader->setUniform("numToGenerate", numToGenerate);
		elapsedTime -= nextGenerationTime;

		glm::vec3 randomSeed = glm::vec3(grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f), grandf(-10.0f, 20.0f));
		updateParticlesShader->setUniform("randomSeed", &randomSeed);
	}

	glEnable(GL_RASTERIZER_DISCARD);
	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, transformFeedbackBuffer);

	glBindVertexArray(vao[curReadBuffer]);
	glEnableVertexAttribArray(1); // Re-enable velocity

	glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particleBuffer[1 - curReadBuffer]);

	glBeginQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, query);
	glBeginTransformFeedback(GL_POINTS);

	glDrawArrays(GL_POINTS, 0, numParticles);

	glEndTransformFeedback();

	glEndQuery(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
	glGetQueryObjectiv(query, GL_QUERY_RESULT, &numParticles);

	curReadBuffer = 1 - curReadBuffer;

	glBindTransformFeedback(GL_TRANSFORM_FEEDBACK, 0);
}

void ParticleTF::renderParticles()
{
	if (!initialized)return;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(0);

	glDisable(GL_RASTERIZER_DISCARD);
	renderParticlesShader->useShader();
	renderParticlesShader->setUniform("matrices->proj", projection);
	renderParticlesShader->setUniform("matrices->view", view);
	renderParticlesShader->setUniform("matrices->myVP", myVP);
	renderParticlesShader->setUniform("quad1", &quad1);
	renderParticlesShader->setUniform("quad2", &quad2);
	glUniform1i(glGetUniformLocation(renderParticlesShader->programHandle, "sampler"), 0);

	glBindVertexArray(vao[curReadBuffer]);
	glDisableVertexAttribArray(1); // Disable velocity, because we don't need it for rendering

	glDrawArrays(GL_POINTS, 0, numParticles);

	glDepthMask(1);
	glDisable(GL_BLEND);
}

void ParticleTF::setMatrices(glm::mat4* inputProjection, glm::mat4* _view, glm::vec3 inputUpVector)
{
	projection = *inputProjection;
	glm::vec3 vView;
	glm::mat4 dView = *_view;
	view = dView;
	vView.x = dView[0][2];
	vView.y = dView[1][2];
	vView.z = dView[2][2];

	vView = glm::normalize(vView);

	quad1 = glm::cross(vView, inputUpVector);
	quad1 = glm::normalize(quad1);
	quad2 = glm::cross(vView, quad1);
	quad2 = glm::normalize(quad2);
}

void ParticleTF::setGeneratorProperties(glm::vec3 a_vGenPosition, glm::vec3 a_vGenVelocityMin, glm::vec3 a_vGenVelocityMax, glm::vec3 a_vGenGravityVector, 
	glm::vec3 a_vGenColor, float a_fGenLifeMin, float a_fGenLifeMax, float a_fGenSize, float fEvery, int a_iNumToGenerate, GLuint _texture)
{
	genPosition = a_vGenPosition;
	genVelocityMin = a_vGenVelocityMin;
	genVelocityRange = a_vGenVelocityMax - a_vGenVelocityMin;

	genGravityVector = a_vGenGravityVector;
	genColor = a_vGenColor;
	genSize = a_fGenSize;

	genLifeMin = a_fGenLifeMin;
	genLifeRange = a_fGenLifeMax - a_fGenLifeMin;

	nextGenerationTime = fEvery;
	elapsedTime = 0.8f;

	numToGenerate = a_iNumToGenerate;
	texture = _texture;
}