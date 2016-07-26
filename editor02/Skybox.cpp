#include "Skybox.h"
#include "ModelManager.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>


void Skybox::Setup(GLuint cubeTextureID) 
{
	m_uCubeTextureID = cubeTextureID;
	
	m_pSkyBoxModel = ModelManager::LoadStaticModel("../Data/Models/Static/skybox.X");
	SetupShader();
}

void Skybox::Draw(glm::mat4& view, glm::mat4& proj, glm::vec3& camPos)
{
	

	glm::mat4 world;
	glm::mat4 rot;

	world = glm::translate(world, glm::vec3(camPos.x, camPos.y - 18000, camPos.z));
	world = glm::scale(world, glm::vec3(300, 300, 300));

	m_SkyBoxShader.Begin();
	m_SkyBoxShader.SetUniform_Matrix(m_WorldMatrixLoc, world);
	m_SkyBoxShader.SetUniform_Matrix(m_ViewMatrixLoc, view);
	m_SkyBoxShader.SetUniform_Matrix(m_ProjectionMatrixLoc, proj);
	
	m_SkyBoxShader.SetUniform_float3(m_CameraPositionLoc, camPos);
	
	
	glActiveTexture(GL_TEXTURE0);
	m_SkyBoxShader.SetUniform_1i("skybox", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_uCubeTextureID);
	
	m_pSkyBoxModel->Draw();
}

void Skybox::SetupShader()
{
	if (m_SkyBoxShader.CreateShader("skybox.vs", "skybox.ps"))
	{
		m_WorldMatrixLoc = glGetUniformLocation(m_SkyBoxShader.GetShaderProgram(), "world");
		m_ViewMatrixLoc = glGetUniformLocation(m_SkyBoxShader.GetShaderProgram(), "view");
		m_ProjectionMatrixLoc = glGetUniformLocation(m_SkyBoxShader.GetShaderProgram(), "projection"); 
		m_CameraPositionLoc = glGetUniformLocation(m_SkyBoxShader.GetShaderProgram(), "CamPos");
	}
	
}

Skybox::~Skybox()
{
	m_pSkyBoxModel->Clear();
}
