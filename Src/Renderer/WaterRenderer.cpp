#include "WaterRenderer.h"


#ifdef _DEBUG
#define CHECK_GL_ERROR(file, line) glCheckError(file, line); 
#else
#define CHECK_GL_ERROR(file, line) 
#endif // !_DEBUG



WaterRenderer& WaterRenderer::get()
{
	static WaterRenderer wR;
	return wR;
}



WaterRenderer::WaterRenderer()
{
	m_defaultShader.createShader("src/shader/DefaultWater.vert", "src/shader/DefaultWater.frag");
}

void WaterRenderer::draw(RenderContext& renderContext, glm::mat4& mvp, float totalTime, bool useDefaultShader)
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	renderContext.vertexArray.bindVAO();

	if (useDefaultShader)
		m_defaultShader.useShader();
	else
		renderContext.shader.useShader();

	CHECK_GL_ERROR(__FILE__, __LINE__);


	if (useDefaultShader)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_defaultShader.ID, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1f(glGetUniformLocation(m_defaultShader.ID, "u_time"), totalTime);
		glUniform1i(glGetUniformLocation(m_defaultShader.ID, "u_texture"), 0);
	}
	else
	{ 
		glUniformMatrix4fv(glGetUniformLocation(renderContext.shader.ID, "u_mvp"), 1, GL_FALSE, glm::value_ptr(mvp));
		glUniform1i(glGetUniformLocation(renderContext.shader.ID, "u_texture"), 0);
	}

	CHECK_GL_ERROR(__FILE__, __LINE__);

	glDrawElements(GL_TRIANGLES, renderContext.numOfIndices, GL_UNSIGNED_INT, (void*)0);

	CHECK_GL_ERROR(__FILE__, __LINE__);

	if (useDefaultShader)
		m_defaultShader.unbindShader();
	else
		renderContext.shader.unbindShader();
	
	renderContext.vertexArray.unbindVAO();

	glDisable(GL_BLEND);
}
