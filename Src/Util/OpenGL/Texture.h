#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <glad/glad.h>
#include <STB/stb_image.h>
#include <iostream>


class Texture
{
public:
	GLuint ID;

	Texture(GLenum textureType, GLenum slot, const char* texPath);
	~Texture();

	void setTextParameterI(GLenum paramName, GLint param);

	void bindTexture();
	void unbindTexture();

	void deleteTexture();

private:
	GLenum m_type;

};



#endif // !TEXTURE_HPP