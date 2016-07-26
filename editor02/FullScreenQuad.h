#ifndef FULL_SCREEN_QUAD_H
#define FULL_SCREEN_QUAD_H
#define GLEW_STATIC
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <GL/glew.h>
#include "Shader.h"

class FullScreenQuad
{
public:
	FullScreenQuad();
	
	void Init();
	void Draw(int screenWidth, int screenHeight);
	void SetTextureID(GLuint textureID);
private:
	GLuint quadVAO = 0;
	GLuint quadVBO;
	Shader shader;
	GLuint screenTextureLoc;
	GLuint textureID;
};


#endif