#pragma once

#include "Block.h"
#include "../../Util/OpenGL/VAO.h"
#include "../../Util/OpenGL/VBO.h"
#include <glm/mat4x4.hpp>
#include "../../Util/OpenGL/Shader.h"


class DebugBlock
{
public:

	DebugBlock(int x, int y, int z);
	~DebugBlock();


	void render(glm::mat4& mvp);

private:

	Block m_block;
	VAO m_vao;
	VBO m_verterPos;
	VBO m_color;
	EBO m_ebo;
	Shader m_shader;

};

