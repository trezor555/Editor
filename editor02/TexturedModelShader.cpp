#include "TexturedModelShader.h"

bool TexturedModelShader::Init()
{
	shader.CreateShader("texturedModel.vs", "texturedModel.ps");

	projectionLoc = glGetUniformLocation(shader.GetShaderProgram(), "projection");
	worldLoc = glGetUniformLocation(shader.GetShaderProgram(), "world");;
	viewLoc = glGetUniformLocation(shader.GetShaderProgram(), "view");;
	baseTextureLoc = glGetUniformLocation(shader.GetShaderProgram(), "baseTexture");;

	return true;
}

void TexturedModelShader::Set()
{
	shader.Begin();

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(m_projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(m_view));
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(m_world));
	glUniform1i(baseTextureLoc, 0);
}

void TexturedModelShader::SetWorldMatrix(glm::mat4& worldMatrix)
{
	
	m_world = worldMatrix;
	glUniformMatrix4fv(worldLoc, 1, GL_FALSE, glm::value_ptr(m_world));
}

void TexturedModelShader::SetProjectionMatrix(glm::mat4& projection)
{
	m_projection = projection;
	
}

void TexturedModelShader::SetViewMatrix(glm::mat4& viewMatrix)
{
	m_view = viewMatrix;
}




