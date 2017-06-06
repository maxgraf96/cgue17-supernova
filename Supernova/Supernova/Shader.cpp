#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <memory>

using namespace supernova;
using namespace std;

Shader::Shader(const string& firstShader, const string& secondShader, bool vertexFragment)
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
	}
	else {
		loadShader(firstShader, GL_VERTEX_SHADER, vertexHandle);
		loadShader(secondShader, GL_GEOMETRY_SHADER, geometryHandle);
	}
}

Shader::Shader(const string& vertexShader, const string& fragmentShader, const string& geometryShader)
	: programHandle(0), vertexHandle(0), fragmentHandle(0), geometryHandle(0) {
	programHandle = glCreateProgram();

	if (programHandle == 0)
	{
		std::cerr << "Failed to create shader program" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	loadShader(vertexShader, GL_VERTEX_SHADER, vertexHandle);
	loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentHandle);
	loadShader(geometryShader, GL_GEOMETRY_SHADER, geometryHandle);
}

Shader::~Shader() {
	glDeleteProgram(programHandle);
	glDeleteShader(vertexHandle);
	glDeleteShader(fragmentHandle);
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
		glShaderSource(handle, 1, &codePtr, nullptr);
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

void Shader::link() {
	//Attach shader to program
	if (vertexHandle != 0) {
		glAttachShader(programHandle, vertexHandle);
	}
	if (fragmentHandle != 0) {
		glAttachShader(programHandle, fragmentHandle);
	}
	if (geometryHandle != 0) {
		glAttachShader(programHandle, geometryHandle);
	}

	//Bind output here
	glBindFragDataLocation(programHandle, 0, "fragColor");

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

void Shader::setUniform(string sName, const glm::vec2 vVector)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform2fv(iLoc, 1, (GLfloat*)&vVector);
}

void Shader::setUniform(string sName, glm::vec3* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform3fv(iLoc, iCount, (GLfloat*)vVectors);
}

void Shader::setUniform(string sName, const glm::vec3 vVector)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform3fv(iLoc, 1, (GLfloat*)&vVector);
}

void Shader::setUniform(string sName, glm::vec4* vVectors, int iCount)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform4fv(iLoc, iCount, (GLfloat*)vVectors);
}

void Shader::setUniform(string sName, const glm::vec4 vVector)
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

void Shader::setUniform(string sName, const glm::mat3 mMatrix)
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

void Shader::setUniform(string sName, const glm::mat4 mMatrix)
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

void Shader::setUniform(string sName, const int iValue)
{
	int iLoc = glGetUniformLocation(programHandle, sName.c_str());
	glUniform1i(iLoc, iValue);
}

void Shader::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4 mModelMatrix)
{
	setUniform(sModelMatrixName, mModelMatrix);
	setUniform(sNormalMatrixName, glm::transpose(glm::inverse(mModelMatrix)));
}

void Shader::SetModelAndNormalMatrix(string sModelMatrixName, string sNormalMatrixName, glm::mat4* mModelMatrix)
{
	setUniform(sModelMatrixName, mModelMatrix);
	setUniform(sNormalMatrixName, glm::transpose(glm::inverse(*mModelMatrix)));
}