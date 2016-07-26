#ifndef TEXTURE2D_H
#define TEXTURE2D_H
#define GLEW_STATIC

#include <GL/glew.h>

class Texture2D
{
public:
	bool CreateTexture(const char*  filePath, bool wrap = true, bool mipmap = true);
	int GetWidth();
	int GetHeight();
	GLuint GetID() const;
	void Release();
private:
	GLuint textureID;
	int width, height;
};

#endif