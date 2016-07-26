#ifndef INSTANCED_STATIC_MODEL_DATA_H
#define INSTANCED_STATIC_MODEL_DATA_H
#include "InstancedStaticMeshSubset.h"
#include "BoundingBox.h"
#include <vector>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>



class InstanceStaticModelData
{
public:
	
	
	bool Load(const char* path);
	void Clear();
	void Draw(unsigned int numInstances, std::vector<glm::mat4>& positions);
	void CreateSubmesh(unsigned int Index, const aiMesh* paiMesh, const aiNode* nd, const aiScene* pScene);
	const InstancedMeshSubset* GetSubset(int index);
	int SubsetCount();
	BoundingBox boundingbox;
private:
	std::vector<InstancedMeshSubset*> meshSubsets;
};

#endif