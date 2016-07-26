#ifndef INSTANCING_SHADER_H
#define INSTANCING_SHADER_H
#include <glm.hpp>
#include "Shader.h"
#include "IShader.h"
class InstancingShader : public IShader
{
public:
	bool Init();
	void SetWorldMatrix(glm::mat4& worldMatrix) {};
	void SetProjectionMatrix(glm::mat4& projection);
	void SetViewMatrix(glm::mat4& viewMatrix);
	void Set();
private:
	GLuint projectionLoc;
	GLuint viewLoc;
	GLuint baseTextureLoc;
	Shader shader;

	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::vec3 m_color;
};

#endif