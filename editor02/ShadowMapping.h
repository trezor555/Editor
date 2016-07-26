#ifndef SHADOW_MAPPING_H
#define SHADOW_MAPPING_H

#define GLEW_STATIC
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Shader.h"
#include "IShader.h"
class ShadowMapping : public IShader
{
public:
	void Init();
	void SetUniformWorldLocation(glm::vec3 wordlPos);
	void BeginDepthPass();
	void EndDepthPass();
	void BeginModelsShadowMapPass(glm::mat4& view, glm::mat4& projection, glm::vec3& cameraPos);
	void EndModelsShadowMapPass();
	void SetWorldMatrix(glm::mat4& worldMatrix);
	void ActivateDepthMapTexture();
	
	GLuint GetDepthMap();
	bool m_depthPassActive;
	GLuint depthMapFBO;
	GLuint depthMap;
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	glm::mat4 worldMatrix;
	GLfloat near_plane = 1.0f, far_plane = 50000;
	Shader depthShader;

	GLuint depthWorldLocation;
	GLuint lightSpaceMatrixLoc;
	glm::vec3 lightPos;
	// Shadowmap
	Shader shadowmapShader;

	GLuint projectionLoc;
	GLuint viewLoc;
	GLuint modeLocl;
	GLuint shadowmapLightSpaceMatrixLoc;

	GLuint diffuseTextureLoc;
	GLuint shadowMapLoc;

	GLuint lightPosLoc;
	GLuint viewPosLoc;
	GLuint shadowsLoc;
};

#endif
