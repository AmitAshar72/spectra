#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include "stb_image.h"
#include "Shader.h"

class Texture
{
	public:
		GLuint ID;
		const char* type;
		GLuint slot;
		GLenum format;
		std::string texPath;

		Texture(const std::string& dir, const char* image, const char* textureType, GLuint slot, GLenum pixelType);

		void TextureUnit(Shader& shader, const char* uniform, GLuint unit);
		void Activate();
		void Bind();
		void Unbind();
		void Delete();
};
#endif 


