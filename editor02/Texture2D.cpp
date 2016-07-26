#include "Texture2D.h"
#include <iostream>

#include <FreeImage.h>

bool Texture2D::CreateTexture(const char* filePath, bool wrap , bool mipmap)
{
	int iWidth, iHeight, iBPP;
	FIBITMAP *dib1 = nullptr;
	

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	FIBITMAP* dib(0);

	fif = FreeImage_GetFileType(filePath, 0); // Check the file signature and deduce its format

	if (fif == FIF_UNKNOWN) // If still unknown, try to guess the file format from the file extension
		fif = FreeImage_GetFIFFromFilename(filePath);

	if (fif == FIF_UNKNOWN) // If still unkown, return failure
		return false;

	if (FreeImage_FIFSupportsReading(fif)) // Check if the plugin has reading capabilities and load the file
		dib = FreeImage_Load(fif, filePath);
	if (!dib)
		return false;

	BYTE* bDataPointer = FreeImage_GetBits(dib); // Retrieve the image data

	width = iWidth = FreeImage_GetWidth(dib); // Get the image width and height
	height = iHeight = FreeImage_GetHeight(dib);
	iBPP = FreeImage_GetBPP(dib);

	// If somehow one of these failed (they shouldn't), return failure
	if (bDataPointer == nullptr || iWidth == 0 || iHeight == 0)
		return false;


	
	GLenum internalFormat;
	GLenum format;
	GLenum type;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	switch (iBPP)
	{
	case 32:
		internalFormat = GL_RGBA;
		format = GL_BGRA;
		type = GL_UNSIGNED_BYTE;
		break;
	case  24:
		internalFormat = GL_RGB;
		format = GL_BGR;
		type = GL_UNSIGNED_BYTE;
		break;
	case 16:
		internalFormat = GL_LUMINANCE16;
		format = GL_LUMINANCE;
		type = GL_UNSIGNED_SHORT;
		break;
	default:
		break;
	}
	
		
	if (wrap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_S);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_TEXTURE_WRAP_T);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}

	if (mipmap)
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, iWidth, iWidth, 0, format, type, bDataPointer);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, iWidth, iWidth, 0, format, type, bDataPointer);
	}
	
	FreeImage_Unload(dib);
	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

GLuint Texture2D::GetID() const
{
	return textureID;
}

void Texture2D::Release()
{
	glDeleteTextures(1, &textureID);
}
