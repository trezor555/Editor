#include "InstancingShader.h"

bool InstancingShader::Init()
{
	shader.CreateShader("instancing.vs", "instancing.ps");

	projectionLoc = glGetUniformLocation(shader.GetShaderProgram(), "projection");
	
	viewLoc = glGetUniformLocation(shader.GetShaderProgram(), "view");
	baseTextureLoc = glGetUniformLocation(shader.GetShaderProgram(), "baseTexture");;

	return true;
}

void InstancingShader::Set()
{
	shader.Begin();
	shader.SetUniform_Matrix(projectionLoc, m_projection);
	shader.SetUniform_Matrix(viewLoc, m_view);
	shader.SetUniform_1i(baseTextureLoc, 0);
}


void InstancingShader::SetProjectionMatrix(glm::mat4& projection)
{
	m_projection = projection;

}

void InstancingShader::SetViewMatrix(glm::mat4& viewMatrix)
{
	m_view = viewMatrix;
}