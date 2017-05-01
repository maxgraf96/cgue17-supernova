#pragma once

#include <glew.h>
#include "SceneObject.hpp"
#include "../Shader.hpp"

#include<memory>
#include<map>
#ifdef SCENE_OBJECT
namespace supernova {
	namespace scene {

		/* Struct for FreeType character */
		struct Character
		{
			GLuint TextureID;
			glm::ivec2 Size;
			glm::ivec2 Bearing;
			GLuint Advance;
		};

		class TextQuad : public SceneObject
		{
		public:
			TextQuad();
			TextQuad(glm::mat4& matrix, Shader* shader);
			virtual ~TextQuad();

			virtual void update(float time_delta, int pressed);
			virtual void draw();
			virtual void draw(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color, std::map<GLchar, Character> chars);

		private:
			GLuint vao;
			GLuint positionBuffer;
			Shader* shader;
		};
	}
}
#endif // SCENE_OBJECT