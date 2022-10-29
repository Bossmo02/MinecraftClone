#include "UI.h"



GLenum inline glCheckError(const char* file, int line)
{
	GLenum errorCode;
	while ((errorCode = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (errorCode)
		{
		case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
		case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
		case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
		case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
		case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
		case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		std::cout << error << " | " << file << " (" << line << ")" << std::endl;
	}
	return errorCode;
}




UI::UI(const char* uiTexturePath, GLenum texSlot, glm::vec2 singleTextureDimensions, glm::vec2 textureDimensions)
	:m_tex(GL_TEXTURE_2D, texSlot, uiTexturePath, false, GL_LINEAR)
{
	glCheckError(__FILE__, __LINE__);

	m_singleTexSize = singleTextureDimensions;
	m_texSize = textureDimensions;

	glCheckError(__FILE__, __LINE__);

	m_uiShader.createShader("src/Shader/UI.vert", "src/Shader/UI.frag");

	glCheckError(__FILE__, __LINE__);

	m_startIndexes = new GLuint[m_elements.size()];
	m_sizeIndexes = new GLuint[m_elements.size()];
}

UI::~UI()
{
}

// position and size range from -1, -1 to 1, 1
void UI::addUIElement(std::string name, glm::vec2 pos, glm::vec2 size, glm::vec2 texIndex)
{
	m_elements.push_back(std::pair<std::string, UIElement>(name, { glm::vec2(pos.x, pos.y), size, texIndex, false }));

	refreshVAO();
}

void UI::setHighlight(std::string nameOfElement)
{
	searchForHighlightedElement();

	for (auto& e : m_elements)
	{
		if (e.first == nameOfElement)
		{
			e.second.elemPos.y += e.second.size.y / 2;
			e.second.isHighlighted = true;
		}
	}

	refreshVAO();
}

void UI::undoHighlight(std::string nameOfElement)
{
	for (auto& e : m_elements)
	{
		if (e.first == nameOfElement)
		{
			e.second.elemPos.y -= e.second.size.y / 2;
			e.second.isHighlighted = false;
		}
	}

	refreshVAO();
}


void UI::render()
{
	glCheckError(__FILE__, __LINE__);

	m_vao.bindVAO();
	m_uiShader.useShader();
	glUniform1i(glGetUniformLocation(m_uiShader.ID, "u_texture"), 1);

	glCheckError(__FILE__, __LINE__);

	glMultiDrawArrays(GL_TRIANGLE_STRIP, (const GLint*)m_startIndexes, (const GLsizei*)m_sizeIndexes, m_elements.size());

	glCheckError(__FILE__, __LINE__);

	m_uiShader.unbindShader();
	m_vao.unbindVAO();
}

void UI::refreshVAO()
{
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> texCoords;

	vertices.reserve(m_elements.size() * 8);
	texCoords.reserve(m_elements.size() * 8);

	for (auto& e : m_elements)
	{
		// top left
		vertices.push_back(e.second.elemPos.x - (e.second.size.x / 2));
		vertices.push_back(e.second.elemPos.y + (e.second.size.y / 2));

		texCoords.push_back( (e.second.texIndex.x * m_singleTexSize.x) / m_texSize.x);
		texCoords.push_back( (e.second.texIndex.y * m_singleTexSize.y) / m_texSize.y);

		// top right
		vertices.push_back(e.second.elemPos.x + (e.second.size.x / 2));
		vertices.push_back(e.second.elemPos.y + (e.second.size.y / 2));

		texCoords.push_back(( (e.second.texIndex.x + 1) * m_singleTexSize.x) / m_texSize.x);
		texCoords.push_back((e.second.texIndex.y * m_singleTexSize.y) / m_texSize.y);

		// bottom left
		vertices.push_back(e.second.elemPos.x - (e.second.size.x / 2));
		vertices.push_back(e.second.elemPos.y - (e.second.size.y / 2));

		texCoords.push_back((e.second.texIndex.x * m_singleTexSize.x) / m_texSize.x);
		texCoords.push_back(( (e.second.texIndex.y + 1) * m_singleTexSize.y) / m_texSize.y);

		// bottom right
		vertices.push_back(e.second.elemPos.x + (e.second.size.x / 2));
		vertices.push_back(e.second.elemPos.y - (e.second.size.y / 2));

		texCoords.push_back(( (e.second.texIndex.x + 1) * m_singleTexSize.x) / m_texSize.x);
		texCoords.push_back(( (e.second.texIndex.y + 1) * m_singleTexSize.y) / m_texSize.y);
	}


	glCheckError(__FILE__, __LINE__);

	m_vboVert.sendData(vertices.data(), sizeof(GLfloat) * vertices.size(), GL_STATIC_DRAW);
	m_vboTex.sendData(texCoords.data(), sizeof(GLfloat) * texCoords.size(), GL_STATIC_DRAW);

	glCheckError(__FILE__, __LINE__);

	m_vao.deleteVAO();

	m_vao.bindVAO();
	m_vao.linkVBO(m_vboVert, 0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	m_vao.linkVBO(m_vboTex, 1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void*)0);
	m_vao.unbindVAO();

	glCheckError(__FILE__, __LINE__);

	m_vboTex.deleteVBO();
	m_vboVert.deleteVBO();

	glCheckError(__FILE__, __LINE__);

	delete[] m_startIndexes;
	delete[] m_sizeIndexes;

	m_startIndexes = new GLuint[m_elements.size()];
	m_sizeIndexes = new GLuint[m_elements.size()];

	for (size_t i = 0; i < m_elements.size(); i++)
	{
		m_startIndexes[i] = i * 4.f;
		m_sizeIndexes[i] = 4.f;
	}
}

void UI::searchForHighlightedElement()
{
	for (auto& e : m_elements)
	{
		if (e.second.isHighlighted)
		{
			e.second.elemPos.y -= e.second.size.y / 2;
			e.second.isHighlighted = false;
		}
	}
}
