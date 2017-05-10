#pragma once
#include <vector>
#include <glm\vec2.hpp>
#include <glm\vec3.hpp>
/* FreeImage is used to load images */
#include <FreeImage.h>

#include "../Shader.hpp"

// Never forget
using namespace supernova;
using namespace std;

class TextureLoader {
public:
	TextureLoader() {}
	GLuint load(const GLchar* path);
	GLuint loadCubemap(std::vector<const GLchar*> faces);
};