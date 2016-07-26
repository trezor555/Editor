#include "SkinnedMeshShader.h"

void SkinnedMeshShader::Init()
{
	shader.CreateShader("skinning.vs", "skinning.fs");
	gWVPLoc = glGetUniformLocation(shader.GetShaderProgram(), "gWVP");
	gWorldLoc = glGetUniformLocation(shader.GetShaderProgram(), "gWorld");
	diffuseTexLoc = glGetUniformLocation(shader.GetShaderProgram(), "diffusetexture");
	for (unsigned int i = 0; i < MAX_BONES; i++) 
	{
		std::string name = "gBones[" + std::to_string(i) + "]";
		gBonesLoc[i] = glGetUniformLocation(shader.GetShaderProgram(), name.c_str());
	}
}

void SkinnedMeshShader::SetWVP(const glm::mat4& WVP)
{
	glUniformMatrix4fv(gWVPLoc, 1, GL_FALSE, glm::value_ptr(WVP));
}

void SkinnedMeshShader::SetWorldMatrix(const glm::mat4& worldM)
{
	glUniformMatrix4fv(gWorldLoc, 1, GL_FALSE, glm::value_ptr(worldM));
}

void SkinnedMeshShader::SetBoneTransform(unsigned int Index, const glm::mat4& Transform)
{
	glUniformMatrix4fv(gBonesLoc[Index], 1, GL_FALSE, glm::value_ptr(Transform));
}

void SkinnedMeshShader::Set()
{
	shader.Begin();
	glActiveTexture(GL_TEXTURE0);
	shader.SetUniform_1i(diffuseTexLoc, 0);
}
