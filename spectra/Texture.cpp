#include "Texture.h"

Texture::Texture(const std::string& dir, const char* image , const char* textureType, GLuint textureSlot, GLenum pixelType)
{
	type = textureType;
	slot = textureSlot;
	texPath = image;
	std::string filename = dir + '/' + image;

	// load and generate the texture
	int width, height, nrChannels;

	glGenTextures(1, &ID);
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, ID);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);

	switch (nrChannels)
	{
		case 1: 
			format = GL_RED;
			break;
		default:
		case 3: 
			format = GL_RGB;
			break;
		case 4: 
			format = GL_RGBA;
			break;
	};

	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, pixelType, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::TextureUnit(Shader& shader, const char* uniform, GLuint unit)
{
	shader.Activate();
	shader.setInt(uniform, unit);
}

void Texture::Activate() 
{
	glActiveTexture(slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Bind()
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}
