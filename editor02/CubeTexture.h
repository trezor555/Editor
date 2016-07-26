#ifndef CUBE_TEXTURE_H
#define CUBE_TEXTURE_H
#define GLEW_STATIC
#include <GL/glew.h>
#include <vector>

class CubeTexture
{
public:
	CubeTexture(std::vector<char*> faces);
	bool Create();
	GLuint GetID();
	void Release();
private:

	std::vector<char*> faces;
	GLuint textureID;

};

#endif