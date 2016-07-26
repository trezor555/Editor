#include "ShadowMapping.h"
const GLuint SHADOW_WIDTH = 4096, SHADOW_HEIGHT = 4096;
void ShadowMapping::Init()
{
	
	glGenFramebuffers(1, &depthMapFBO);
	
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	

	depthShader.CreateShader("depth.vs", "depth.ps");
	depthWorldLocation = glGetUniformLocation(depthShader.GetShaderProgram(), "world");
	lightSpaceMatrixLoc = glGetUniformLocation(depthShader.GetShaderProgram(), "lightSpaceMatrix");

	shadowmapShader.CreateShader("shadowMap.vs", "shadowMap.ps");
	projectionLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "projection");
	viewLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "view");
	modeLocl = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "model");
	shadowmapLightSpaceMatrixLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "lightSpaceMatrix");

	diffuseTextureLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "diffuseTexture");
	shadowMapLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "shadowMap");

	lightPosLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "lightPos");
	viewPosLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "viewPos");
	shadowsLoc = glGetUniformLocation(shadowmapShader.GetShaderProgram(), "shadows");
}

float lps = 0.0;
void ShadowMapping::BeginDepthPass()
{
	m_depthPassActive = true;
	depthShader.Begin();
	lps += 0.008;
	lightPos = glm::vec3(lps, 1000, -10.0f);
	lightProjection = glm::ortho(-2500.0f, 2500.0f, -2500.0f, 2500.0f, near_plane, far_plane);
	lightView = glm::lookAt(lightPos, glm::vec3(0.0, 0, 0), glm::vec3(0, 1.0, 0));
	lightSpaceMatrix = lightProjection * lightView;
	
	glUniformMatrix4fv(lightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMapping::EndDepthPass()
{
	m_depthPassActive = false;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint ShadowMapping::GetDepthMap()
{
	return depthMap;
}

void ShadowMapping::BeginModelsShadowMapPass(glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPos)
{
	shadowmapShader.Begin();
	GLboolean shadows = true;
	glUniform1i(diffuseTextureLoc, 0);
	glUniform1i(shadowMapLoc, 1);
	
	
	glUniformMatrix4fv(shadowmapLightSpaceMatrixLoc, 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniform1i(shadowsLoc, shadows);
	glUniform3f(viewPosLoc, cameraPos.x, cameraPos.y, cameraPos.z);
	glUniform3f(lightPosLoc, lightPos.x, lightPos.y, lightPos.z);
}

void ShadowMapping::EndModelsShadowMapPass()
{

}

void ShadowMapping::SetWorldMatrix(glm::mat4& worldMatrix)
{
	if (m_depthPassActive)
	{
		glUniformMatrix4fv(depthWorldLocation, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	}
	else
	{
		glUniformMatrix4fv(modeLocl, 1, GL_FALSE, glm::value_ptr(worldMatrix));
	}
	
	//his->worldMatrix = worldMatrix;
}

void ShadowMapping::SetUniformWorldLocation(glm::vec3 wordlPos)
{

}

void ShadowMapping::ActivateDepthMapTexture()
{
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, depthMap);
}
