#include "TextureManager.h"

std::map<std::string, Texture2D*> TextureManager::m_Texture2D_Map;

Texture2D* TextureManager::LoadTexture2D(const char* path, bool wrap, bool mipmap)
{
	std::map<std::string, Texture2D*>::iterator texIterator;
	
	texIterator = m_Texture2D_Map.find(path);
	
	if (texIterator == m_Texture2D_Map.end())
	{
		Texture2D* newTexture = new Texture2D();
		if (newTexture->CreateTexture(path))
		{
			m_Texture2D_Map.insert(std::pair<std::string, Texture2D*>(path, newTexture));
			return newTexture;
		}
		else
		{
			LoadTexture2D("../Data/Textures/Diffuse/noTexture.jpg");
		}
	}
	else
	{
		return texIterator->second;
	}

	return nullptr;
}
