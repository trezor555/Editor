#ifndef COLOR_SHADER_H
#define COLOR_SHADER_H
#include "Shader.h"
#include "IShader.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

class VertexColorShader : public IShader
{
public:
	VertexColorShader() {};
	bool Init();
	void SetWorldMatrix(glm::mat4& worldMatrix);
	void SetProjectionMatrix(glm::mat4& projection);
	void SetViewMatrix(glm::mat4& viewMatrix);
	void Set();
private:
	GLuint worldLoc;
	GLuint viewLoc;
	GLuint projectionLoc;
	Shader shader;

	glm::mat4 m_projection;
	glm::mat4 m_world;
	glm::mat4 m_view;
	glm::vec3 m_color;

};


#endif