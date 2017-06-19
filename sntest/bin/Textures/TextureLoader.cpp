#include "TextureLoader.hpp"

/* Function for loading an image from a file, uses the FreeImage library */
GLint TextureLoader::load(const char* path) {
	unsigned int textureId;
	// Link texture to id
	glGenTextures(1, &textureId);

	auto bitmap = FreeImage_Load(FreeImage_GetFileType(path, 0), path);
	int bitsPerPixel = FreeImage_GetBPP(bitmap);

	// Convert to 32bit image iff it not already is
	FIBITMAP* bitmap32;
	if (bitsPerPixel == 32) {
		bitmap32 = bitmap;
	}
	else {
		bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
	}

	int width = FreeImage_GetWidth(bitmap32);
	int height = FreeImage_GetHeight(bitmap32);

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGBA,
		width,
		height,
		0,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		(GLvoid*)FreeImage_GetBits(bitmap32));
	
	glGenerateMipmap(GL_TEXTURE_2D);

	// Params
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Unbind
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureId;
}

/* Used to bind skybox texture to skybox object */
GLuint TextureLoader::loadCubemap(std::vector<const GLchar*> faces) {
	GLuint textureId;
	glGenTextures(1, &textureId);

	int width, height;
	char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureId);
	for (GLuint i = 0; i < faces.size(); i++) {
		/* Load texture */
		FIBITMAP* bitmap = FreeImage_Load(FreeImage_GetFileType(faces[i], 0), faces[i]);
		int bitsPerPixel = FreeImage_GetBPP(bitmap);

		// Convert to 32bit image iff it not already is
		FIBITMAP* bitmap32;
		if (bitsPerPixel == 32) {
			bitmap32 = bitmap;
		}
		else {
			bitmap32 = FreeImage_ConvertTo32Bits(bitmap);
		}

		int width = FreeImage_GetWidth(bitmap32);
		int height = FreeImage_GetHeight(bitmap32);

		// Link texture to id
		glGenTextures(1, &textureId);
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
			0,
			GL_RGBA8,
			width,
			height,
			0,
			GL_BGRA,
			GL_UNSIGNED_BYTE,
			(void*)FreeImage_GetBits(bitmap32));
		// Unbind and unload
		glBindTexture(GL_TEXTURE_2D, 0);
		FreeImage_Unload(bitmap32);
		if (bitsPerPixel != 32)
		{
			FreeImage_Unload(bitmap);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureId;
}