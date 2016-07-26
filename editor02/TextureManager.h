#ifndef TEXTURE_MANAGER_H
#define TEXTURE_MANAGER_H

#include <map>
#include "Texture2D.h"
enum TEXTURE_TYPE
{
	TEXTURE_DIFFUSE,
	TEXTURE_SPECULAR,
	TEXTURE_NORMAL
};

struct Texture
{
	GLuint textureID;
	TEXTURE_TYPE textureType;
};

class TextureManager
{
public:
	
	static Texture2D* LoadTexture2D(const char* path, bool wrap = true, bool mipmap = true);
private:
	static std::map<std::string, Texture2D*> m_Texture2D_Map;
};

#endif