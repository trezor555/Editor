#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "Shader.h"
#include "IShader.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <map>
class ShaderManager
{
public:
	void UpdateWorldMatrix(glm::mat4& worldMatrix);
	void UpdateProjectionMatrix(glm::mat4& projectionMatrix);
	void UpdateViewMatrix(glm::mat4& viewMatrix);

	void Init();

private:
	glm::mat4 m_WorldMatrix;
	glm::mat4 m_ProjectionMatrix;
	glm::mat4 m_ViewMatrix;

	std::map<std::string, IShader*> m_Shaders;
};

#endif