#include "DebugBlock.h"

#include <vector>


DebugBlock::DebugBlock(int x, int y, int z)
	: m_block(x, y, z, BLOCK_ID::STONE)
{
	m_shader.createShader("src/shader/Debug.vert", "src/shader/Debug.frag");

	m_block.setFaceVisible(FACES_TO_DISPLAY::ALL);
	std::vector<BlockMeshData> data = m_block.getVisibleMesh();

	// load data to vbos
	for (size_t i = 0; i < data.size(); ++i)
	{

	}

	m_vao.bindVAO();
	//m_vao.linkVBO(m_vbo, 0, m_vbo.)
}

DebugBlock::~DebugBlock()
{
	m_shader.deleteShader();
	m_vao.deleteVAO();
	m_verterPos.deleteVBO();
	m_color.deleteVBO();
	m_ebo.deleteEBO();
}

void DebugBlock::render(glm::mat4& mvp)
{

}
