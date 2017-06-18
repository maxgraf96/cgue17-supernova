#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <memory>

using namespace supernova;
using namespace std;

Shader::Shader() {};

Shader::Shader(const std::string& firstShader, const std::string& secondShader, bool vertexFragment)
	: programHandle(0), vertexHandle(0), fragmentHandle(0), geometryHandle(0) {
	programHandle = glCreateProgram();

	if (programHandle == 0)
	{
		std::cerr << "Failed to create shader program" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	if (vertexFragment) {
		loadShader(firstShader, GL_VERTEX_SHADER, vertexHandle);
		loadShader(secondShader, GL_FRAGMENT_SHADER, fragmentHandle);

		// new
		attachToShaderProgram(vertexHandle);
		attachToShaderProgram(fragmentHandle);
	}
	else {
		loadShader(firstShader, GL_VERTEX_SHADER, vertexHandle);
		loadShader(secondShader, GL_GEOMETRY_SHADER, geometryHandle);

		attachToShaderProgram(vertexHandle);
		attachToShaderProgram(geometryHandle);
	}
}

Shader::Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader)
	: programHandle(0), vertexHandle(0), fragmentHandle(0), geometryHandle(0) {
	programHandle = glCreateProgram();

	if (programHandle == 0)
	{
		std::cerr << "Failed to create shader program" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	loadShader(vertexShader, GL_VERTEX_SHADER, vertexHandle);
	loadShader(geometryShader, GL_GEOMETRY_SHADER, geometryHandle);
	loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentHandle);

	attachToShaderProgram(vertexHandle);
	attachToShaderProgram(geometryHandle);
	attachToShaderProgram(fragmentHandle);
}

Shader::~Shader() {
	glDeleteProgram(programHandle);
	glDeleteShader(vertexHandle);
	glDeleteShader(fragmentHandle);
	glDeleteShader(geometryHandle);
}

void Shader::useShader() const {
	glUseProgram(programHandle);
}

void Shader::loadShader(const std::string& shader, GLenum shaderType, GLuint& handle) {
	std::ifstream shaderFile(shader);

	if (shaderFile.good()) {
		//read file into string
		std::string code = std::string(std::istreambuf_iterator<char>(shaderFile), std::istreambuf_iterator<char>());
		shaderFile.close();

		//generate shader handle
		handle = glCreateShader(shaderType);

		if (handle == 0)
		{
			std::cerr << "Failed to create shader" << std::endl;
			system("PAUSE");
			exit(EXIT_FAILURE);
		}

		//compile shader
		auto codePtr = code.c_str();
		glShaderSource(handle, 1, &codePtr, NULL);
		glCompileShader(handle);

		//test for compilation errors
		GLint succeded;
		glGetShaderiv(handle, GL_COMPILE, &succeded);

		if (succeded == GL_FALSE || !glIsShader(handle)) {
			GLint logSize;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &logSize);

			auto message = std::make_unique<char[]>(logSize);
			glGetShaderInfoLog(handle, logSize, nullptr, message.get());

			std::cerr << "Failed to compile shader" << std::endl;
			std::cerr << message.get() << std::endl;
			system("PAUSE");

			message.reset(nullptr);
			exit(EXIT_FAILURE);
		}
	}
	else {
		std::cerr << "Failed to load shader" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}
}

void Shader::attachToShaderProgram(GLuint handle) {
	if (handle != 0) {
		glAttachShader(programHandle, handle);
	}
}

void Shader::link() {

	//Bind output here
	//glBindFragDataLocation(programHandle, 0, "fragColor");

	//link programs
	glLinkProgram(programHandle);

	//check for errors
	GLint succeded;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &succeded);

	if (succeded == GL_FALSE) {
		GLint logSize;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &logSize);

		auto message = std::make_unique<char[]>(logSize);
		glGetProgramInfoLog(programHandle, logSize, nullptr, message.get());

		std::cerr << "Failed to link shader program" << std::endl;
		std::cerr << message.get() << std::endl;
		system("PAUSE");

		message.reset(nullptr);
		exit(EXIT_FAILURE);
	}
}

void Shader::setLightSources(supernova::scene::DirectionalLight dirLights[], supernova::scene::PointLight pointLights[], supernova::scene::Camera* camera) {

	// Directional lights
	for (GLuint i = 0; i < 12; i++) {
		string number = to_string(i);

		if (!dirLights[i].isInitialized()) continue;
		// Set initialized to true -> the shader will process it
		glUniform1i(glGetUniformLocation(programHandle, ("dirLights[" + number + "].initialized").c_str()), 1);

		// Directional light
		GLint dirLightDirectionLoc = glGetUniformLocation(programHandle, ("dirLights[" + number + "].direction").c_str());
		GLint dirLightAmbientLoc = glGetUniformLocation(programHandle, ("dirLights[" + number + "].ambient").c_str());
		GLint dirLightDiffuseLoc = glGetUniformLocation(programHandle, ("dirLights[" + number + "].diffuse").c_str());
		GLint dirLightSpecularLoc = glGetUniformLocation(programHandle, ("dirLights[" + number + "].specular").c_str());

		glUniform3f(dirLightDirectionLoc, dirLights[i].getDirection().x, dirLights[i].getDirection().y, dirLights[i].getDirection().z);
		glUniform3f(dirLightAmbientLoc, dirLights[i].getAmbient().x, dirLights[i].getAmbient().y, dirLights[i].getAmbient().z);
		glUniform3f(dirLightDiffuseLoc, dirLights[i].getDiffuse().x, dirLights[i].getDiffuse().y, dirLights[i].getDiffuse().z);
		glUniform3f(dirLightSpecularLoc, dirLights[i].getSpecular().x, dirLights[i].getSpecular().y, dirLights[i].getSpecular().z);
	}

	// Point lights
	for (GLuint i = 0; i < 20; i++)
	{
		supernova::scene::PointLight test = pointLights[i];
		if (!pointLights[i].isInitialized()) continue;
		string number = to_string(i);

		// Set initialized to true -> the shader will process it
		glUniform1i(glGetUniformLocation(programHandle, ("pointLights[" + number + "].initialized").c_str()), 1);

		// Position
		glUniform3f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].position").c_str()),
			pointLights[i].getPosition().x, pointLights[i].getPosition().y, pointLights[i].getPosition().z);

		// Color
		glUniform3f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].ambient").c_str()),
			pointLights[i].getAmbient().r, pointLights[i].getAmbient().g, pointLights[i].getAmbient().b);
		glUniform3f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].diffuse").c_str()),
			pointLights[i].getDiffuse().r, pointLights[i].getDiffuse().g, pointLights[i].getDiffuse().b);
		glUniform3f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].specular").c_str()),
			pointLights[i].getSpecular().r, pointLights[i].getSpecular().g, pointLights[i].getSpecular().b);

		// Attenuation
		glUniform1f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].constant").c_str()),
			pointLights[i].getConstant());
		glUniform1f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].linear").c_str()),
			pointLights[i].getLinear());
		glUniform1f(glGetUniformLocation(programHandle, ("pointLights[" + number + "].quadratic").c_str()),
			pointLights[i].getQuadratic());
	}

	// Camera position
	GLuint cameraPosLoc = glGetUniformLocation(programHandle, "cameraPos");
	glUniform3f(cameraPosLoc, camera->getPosition().x, camera->getPosition().y, camera->getPosition().z);
};


void Shader::setUniform(string sName, float* fValues, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform1fv(iLoc, iCount, fValues);
}

void Shader::setUniform(string sName, const float fValue)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform1fv(iLoc, 1, &fValue);
}

// Setting vectors

void Shader::setUniform(string name, glm::vec3* vector) {
	int iLoc = glGetUniformLocation(programHandle, name.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*) &vector);
}

void Shader::setUniform(string sName, glm::vec2* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform2fv(iLoc, iCount, (GLfloat*)vVectors);
}

void Shader::setUniform(string sName, const glm::vec2 &vVector)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}

void Shader::setUniform(string sName, glm::vec3* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}

void Shader::setUniform(string sName, const glm::vec3 &vVector)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void Shader::setUniform(string sName, glm::vec4* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}

void Shader::setUniform(string sName, const glm::vec4 &vVector)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform4fv(iLoc, 1, (GLfloat*)&vVector);
}

// Setting 3x3 matrices

void Shader::setUniform(string sName, glm::mat3* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniformMatrix3fv(iLoc, iCount, false, (GLfloat*)mMatrices);
}

void Shader::setUniform(string sName, const glm::mat3 &mMatrix)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniformMatrix3fv(iLoc, 1, false, (GLfloat*)&mMatrix);
}

// Setting 4x4 matrices

void Shader::setUniform(string sName, glm::mat4* mMatrices, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniformMatrix4fv(iLoc, iCount, false, (GLfloat*)mMatrices);
}

void Shader::setUniform(std::string sName, const glm::mat4 &mMatrix)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniformMatrix4fv(iLoc, 1, false, (GLfloat*)&mMatrix);
}

// Setting integers

void Shader::setUniform(string sName, int* iValues, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform1iv(iLoc, iCount, iValues);
}

void Shader::setUniform(string sName, const int &iValue)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform1i(iLoc, iValue);
}

//void Shader::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix)
//{
//	setUniform(sModelMatrixName, mModelMatrix);
//	setUniform(sNormalMatrixName, glm::transpose(glm::inverse(mModelMatrix)));
//}

//void Shader::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4* mModelMatrix)
//{
//	setUniform(sModelMatrixName, mModelMatrix);
//	setUniform(sNormalMatrixName, glm::transpose(glm::inverse(*mModelMatrix)));
//}