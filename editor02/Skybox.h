#ifndef SKYBOX_H
#define SKYBOX_H
#define GLEW_STATIC
#include <GL/glew.h>
#include "Shader.h"
#include "CubeTexture.h"
#include "StaticMesh.h"

class Skybox
{
public:
	~Skybox();
	void Setup(GLuint cubeTextureID);
	void Draw(glm::mat4& view, glm::mat4& proj, glm::vec3& camPos);
private:
	void SetupShader();

	GLuint m_WorldMatrixLoc;
	GLuint m_ViewMatrixLoc;
	GLuint m_ProjectionMatrixLoc;
	GLuint m_uCubeTextureID;
	GLuint m_CameraPositionLoc;
	Shader m_SkyBoxShader;

	StaticMesh* m_pSkyBoxModel;
	
};

#endif