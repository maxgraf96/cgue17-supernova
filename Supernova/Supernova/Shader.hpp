#pragma once

#include <string>
#include <glew.h>
#include <glfw3.h>
#include <glm\glm.hpp>
#include "Lights\Light.hpp"
#include "Scene\Camera.hpp"

using namespace std;

namespace supernova
{
	class Shader
	{
	public:
		Shader();
		Shader(const std::string& firstShader, const std::string& secondShader, bool vertexFragment);
		Shader(const std::string& vertexShader, const std::string& fragmentShader, const std::string& geometryShader);
		~Shader();

		void attachToShaderProgram(GLuint handle);
		void link();
		void useShader() const;
		GLuint programHandle;

		void setLightSources(supernova::scene::DirectionalLight dirLight, supernova::scene::PointLight pointLights[], supernova::scene::Camera* camera);

		/* uniform set functions from http://www.mbsoftworks.sk/index.php?page=tutorials&series=1&tutorial=26 */

		void Shader::setUniform(std::string name, glm::vec3* vector);

		void Shader::setUniform(std::string sName, float* fValues, int iCount);

		void Shader::setUniform(std::string sName, const float fValue);

		// Setting vectors

		void Shader::setUniform(std::string sName, glm::vec2* vVectors, int iCount);

		void Shader::setUniform(std::string sName, const glm::vec2 &vVector);

		void Shader::setUniform(std::string sName, glm::vec3* vVectors, int iCount);

		void Shader::setUniform(std::string sName, const glm::vec3 &vVector);

		void Shader::setUniform(std::string sName, glm::vec4* vVectors, int iCount);

		void Shader::setUniform(std::string sName, const glm::vec4 &vVector);

		// Setting 3x3 matrices

		void Shader::setUniform(std::string sName, glm::mat3* mMatrices, int iCount);

		void Shader::setUniform(std::string sName, const glm::mat3 &mMatrix);

		// Setting 4x4 matrices

		void Shader::setUniform(std::string sName, glm::mat4* mMatrices, int iCount);

		void Shader::setUniform(std::string sName, const glm::mat4 &mMatrix);

		// Setting integers

		void Shader::setUniform(std::string sName, int* iValues, int iCount);

		void Shader::setUniform(std::string sName, const int &iValue);

		//void Shader::SetModelAndNormalMatrix(std::string sModelMatrixName, std::string sNormalMatrixName, glm::mat4 mModelMatrix);

		//void Shader::SetModelAndNormalMatrix(std::string sModelMatrixName, std::string sNormalMatrixName, glm::mat4* mModelMatrix);

	private:
		void loadShader(const std::string& shader, GLenum shaderType, GLuint& handle);

		GLuint vertexHandle;
		GLuint fragmentHandle;
		GLuint geometryHandle;
	};
}