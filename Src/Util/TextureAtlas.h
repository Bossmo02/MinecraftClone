#ifndef TEXTURE_ATLAS_HPP
#define TEXTURE_ATLAS_HPP

#include <glad/glad.h>
#include <STB/stb_image.h>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG


class TextureAtlas
{
public:

	TextureAtlas(unsigned int countImgsX, unsigned int countImgsY, const char* atlasPath);
	~TextureAtlas();

	void bindTexture();
	void unbindTexture();
	void deleteTexture();

private:

	GLuint m_texID;
	int m_width, m_height, m_singleWidth, m_singleHeight;

};



#endif // !TEXTURE_ATLAS_HPP
