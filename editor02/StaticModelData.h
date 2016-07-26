#ifndef STATIC_MODEL_DATA_H
#define STATIC_MODEL_DATA_H
#include "StaticMeshSubset.h"
#include "BoundingBox.h"
#include <vector>
#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#include "VertexDataTypes.h"

class StaticModelData
{
public:
	
	bool Load(const char* path);
	void Clear();
	void Draw();
	void CreateSubmesh(unsigned int Index, const aiMesh* paiMesh, const aiNode* nd, const aiScene* pScene);
	const MeshSubset* GetSubset(int index);
	int SubsetCount();
	BoundingBox boundingbox;
	unsigned int GetVertexCount() const;
	unsigned int GetIndexCount() const;
	glm::vec3& GetVertices();
	unsigned int& GetIndices();
private:
	std::vector<MeshSubset*> meshSubsets;
	std::vector<glm::vec3> m_vertices;
	std::vector<unsigned int> m_indices;
};

#endif