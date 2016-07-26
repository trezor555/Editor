#ifndef CREATE_MESH_IMAGE_H
#define CREATE_MESH_IMAGE_H

#include "StaticMesh.h"
#include "IShader.h"

class CreateMeshImage
{
public:
	void CreateImage(StaticMesh* mesh, IShader& shader);
};

#endif