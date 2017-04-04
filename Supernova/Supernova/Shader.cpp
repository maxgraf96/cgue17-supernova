#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <memory>

using namespace supernova;
using namespace std;

Shader::Shader(const string& vertexShader, const string& fragmentShader)
	: programHandle(0), vertexHandle(0), fragmentHandle(0) {
	programHandle = glCreateProgram();

	if (programHandle == 0)
	{
		std::cerr << "Failed to create shader program" << std::endl;
		system("PAUSE");
		exit(EXIT_FAILURE);
	}

	loadShader(vertexShader, GL_VERTEX_SHADER, vertexHandle);
	loadShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentHandle);

	link();
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
	glAttachShader(programHandle, vertexHandle);
	glAttachShader(programHandle, fragmentHandle);

	//Bind output here

	//link programs
	glLinkProgram(programHandle);

	//check for errors
	GLint succeded;
	glGetProgramiv(programHandle, GL_LINK_STATUS, &succeded);

	if (!succeded) {
		GLint logSize;
		glGetShaderiv(programHandle, GL_INFO_LOG_LENGTH, &logSize);

		auto message = std::make_unique<char[]>(logSize);
		glGetProgramInfoLog(programHandle, logSize, nullptr, message.get());

		std::cerr << "Failed to compile shader" << std::endl;
		std::cerr << message.get() << std::endl;
		system("PAUSE");

		message.reset(nullptr);
		exit(EXIT_FAILURE);
	}
}