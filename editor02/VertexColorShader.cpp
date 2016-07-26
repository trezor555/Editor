#include "VertexColorShader.h"

bool VertexColorShader::Init()
{
	shader.CreateShader("vColor.vs", "vColor.ps");
	
	worldLoc = glGetUniformLocation(shader.GetShaderProgram(), "world");
	viewLoc = glGetUniformLocation(shader.GetShaderProgram(), "view");;
	projectionLoc = glGetUniformLocation(shader.GetShaderProgram(), "projection");
	return true;
}

void VertexColorShader::Set()
{
	shader.Begin();
	shader.SetUniform_Matrix(projectionLoc, m_projection);
	shader.SetUniform_Matrix(viewLoc, m_view);
}

void VertexColorShader::SetWorldMatrix(glm::mat4& worldMatrix)
{
	shader.SetUniform_Matrix(worldLoc, worldMatrix);
}

void VertexColorShader::SetProjectionMatrix(glm::mat4& projection)
{
	m_projection = projection;
}

void VertexColorShader::SetViewMatrix(glm::mat4& viewMatrix)
{
	m_view = viewMatrix;
}




