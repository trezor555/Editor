#include "CubeTexture.h"
#include <iostream>
#include <SOIL.h>
CubeTexture::CubeTexture(std::vector<char*> faces)
{
	this->faces = faces;
}

bool CubeTexture::Create()
{
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0);

	int width, height;
	unsigned char* image;

	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	for (GLuint i = 0; i < faces.size(); i++)
	{
		std::string fullPath = "../Data/Textures/Skybox/";
		fullPath += faces[i];


		image = SOIL_load_image(fullPath.c_str(), &width, &height, 0, SOIL_LOAD_RGB);

		if (!image)
		{
			//std::cout << "Error loading texture file: " << fullPath.c_str() << std::endl;
			return false;
		}


		glTexImage2D(
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
			GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image
			);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return true;
}

GLuint CubeTexture::GetID()
{
	return textureID;
}

void CubeTexture::Release()
{
	glDeleteTextures(1, &textureID);
}
