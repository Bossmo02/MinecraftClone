#include "Texture.h"

Texture::Texture(GLenum textureType, GLenum textureSlot, const char* texPath, bool flip, GLint filter)
{
	m_type = textureType;
	slot = textureSlot;

	glGenTextures(1, &ID);
	glActiveTexture(textureSlot);
	glBindTexture(m_type, ID);
	

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);

	// load and generate the texture
	int texWidth, texHeight, texChannels;
	stbi_set_flip_vertically_on_load(flip);
	unsigned char* data = stbi_load(texPath, &texWidth, &texHeight, &texChannels, 4);

	if (data)
	{
		if(texChannels == 3)
			glTexImage2D(m_type, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if(texChannels == 4)
			glTexImage2D(m_type, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);


		glGenerateMipmap(m_type);

	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

Texture::~Texture()
{
	glDeleteTextures(1, &ID);
}

void Texture::setTextParameterI(GLenum paramName, GLint param)
{
	// set the texture wrapping/filtering options (on the currently bound texture object)
	bindTexture();
	glTexParameteri(m_type, paramName, param);
	unbindTexture();
}

void Texture::bindTexture()
{
	glBindTexture(m_type, ID);
}

void Texture::unbindTexture()
{
	glBindTexture(m_type, 0);
}

void Texture::deleteTexture()
{
	glDeleteTextures(1, &ID);
}
