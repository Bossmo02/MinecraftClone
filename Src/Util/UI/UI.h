#ifndef UI_HPP
#define UI_HPP

#include <glm/vec2.hpp>
#include <vector>
#include <array>

#include "../OpenGL/Texture.h"
#include "../OpenGL/Shader.h"
#include "../OpenGL/VAO.h"
#include "../OpenGL/VBO.h"



class UI
{
public:
	UI(const char* uiTexturePath, GLenum texSlot, glm::vec2 singleTextureDimensions, glm::vec2 textureDimensions);
	~UI();

	void addUIElement(std::string name, glm::vec2 pos, glm::vec2 size, glm::vec2 texIndex);
	void setHighlight(std::string nameOfElement);
	void undoHighlight(std::string nameOfElement);
	void render();

private:

	void refreshVAO();
	void searchForHighlightedElement();

	struct UIElement
	{
		glm::vec2 elemPos;
		glm::vec2 size;
		glm::vec2 texIndex;
		bool isHighlighted;
	};

	std::vector<std::pair<std::string, UIElement>> m_elements;
	GLuint* m_startIndexes;
	GLuint* m_sizeIndexes;

	glm::vec2 m_singleTexSize;
	glm::vec2 m_texSize;

	Shader m_uiShader;
	Texture m_tex;
	VAO m_vao;
	VBO m_vboVert;
	VBO m_vboTex;
};


#endif // !UI_HPP