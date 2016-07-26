#ifndef TEXTURED_MODEL_SHADER_H
#define TEXTURED_MODEL_SHADER_H

#include "IShader.h"
#include "Shader.h"

class TexturedModelShader : public IShader
{
public:
	TexturedModelShader() {};
	bool Init();
	void SetWorldMatrix(glm::mat4& worldMatrix);
	void SetProjectionMatrix(glm::mat4& projection);
	void SetViewMatrix(glm::mat4& viewMatrix);
	void Set();
private:
	GLuint projectionLoc;
	GLuint worldLoc;
	GLuint viewLoc;
	GLuint baseTextureLoc;
	Shader shader;

	glm::mat4 m_projection;
	glm::mat4 m_world;
	glm::mat4 m_view;
	glm::vec3 m_color;
};

#endif
