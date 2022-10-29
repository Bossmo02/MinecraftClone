#include "TextureAtlas.h"

TextureAtlas::TextureAtlas(unsigned int countImgsX, unsigned int countImgsY, const char* atlasPath)
{

	glGenTextures(1, &m_texID);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_texID);


	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int texChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(atlasPath, &m_width, &m_height, &texChannels, 0);

	if (data)
	{
		if (texChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		else if (texChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);


		glGenerateMipmap(GL_TEXTURE_2D);

	}
	else
	{
#ifdef _DEBUG
		std::cout << "Failed to load texture" << std::endl;
#endif // _DEBUG

	}
	stbi_image_free(data);

	m_singleWidth = m_width / countImgsX;
	m_singleHeight = m_height / countImgsY;
}
