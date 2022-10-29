#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <STB/stb_image.h>
#include <iostream>


class Texture
{
public:
	GLuint ID;
	GLenum slot;

	Texture(GLenum textureType, GLenum textureSlot, const char* texPath, bool flip = true, GLint filter = GL_NEAREST);
	~Texture();

	void setTextParameterI(GLenum paramName, GLint param);

	void bindTexture();
	void unbindTexture();

	void deleteTexture();

private:
	GLenum m_type;
};



#endif // !TEXTURE_HPP