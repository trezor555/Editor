#include "InstancedStaticModelData.h"

#include "TextureManager.h"


bool InstanceStaticModelData::Load(const char* path)
{
	std::ifstream ifile(path);
	if (!ifile)
	{
		std::cout << "Mesh file cannot found" << path << std::endl;
		return false;
	}

	Assimp::Importer Importer;
	const aiScene* pScene = Importer.ReadFile(path, aiProcessPreset_TargetRealtime_Fast);


	for (unsigned int i = 0; i < pScene->mNumMeshes; i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		if (pScene->mRootNode->mChildren == 0)
		{
			CreateSubmesh(i, paiMesh, pScene->mRootNode, pScene);

		}
		else
		{
			CreateSubmesh(i, paiMesh, pScene->mRootNode->mChildren[i], pScene);
		}
	}

	boundingbox.Init();
	return true;
}

void InstanceStaticModelData::Clear()
{
	for (int i = 0; i < meshSubsets.size(); ++i)
	{
		meshSubsets[i]->Clear();
	}
}

void InstanceStaticModelData::CreateSubmesh(unsigned int Index, const aiMesh* paiMesh, const aiNode* nd, const aiScene* pScene)
{
	InstancedMeshSubset* subset = new InstancedMeshSubset();

	Vertex* meshVertices = new Vertex[paiMesh->mNumVertices];
	unsigned int* meshIndices = new unsigned int[paiMesh->mNumFaces * 3];

	const aiMaterial* pMaterial = pScene->mMaterials[paiMesh->mMaterialIndex];

	if (pMaterial != nullptr)
	{
		// diffuse texture

		if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0)
		{
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
			{
				std::string FullPath = Path.data;
				FullPath = "../Data/Textures/Diffuse/" + FullPath;
				Texture2D* tex2d = TextureManager::LoadTexture2D(FullPath.c_str());

				if (tex2d != nullptr)
				{
					Texture tex;
					tex.textureID = tex2d->GetID();
					tex.textureType = TEXTURE_DIFFUSE;
					subset->AddTexture(tex);
				}
				
			}
		}
		else
		{

			std::string FullPath = "../Data/Textures/Diffuse/noTexture.jpg";
			Texture2D* tex2d = TextureManager::LoadTexture2D(FullPath.c_str());
			Texture tex;
			tex.textureID = tex2d->GetID();
			tex.textureType = TEXTURE_DIFFUSE;
			subset->AddTexture(tex);
		}

		// normal texture
		if (pMaterial->GetTextureCount(aiTextureType_NORMALS) > 0)
		{
			aiString Path;

			if (pMaterial->GetTexture(aiTextureType_NORMALS, 0, &Path, nullptr, nullptr, nullptr, nullptr, nullptr) == AI_SUCCESS)
			{

				/*std::string FullPath = Path.data;

				mesh->m_Entries[Index].normalTexture = new D3D9Texture();
				if (!mesh->m_Entries[Index].normalTexture->Load("texture/" + FullPath, m_D3DDevice))
				{
				delete mesh->m_Entries[Index].normalTexture;
				mesh->m_Entries[Index].normalTexture = normalTexture;
				}*/
			}
		}

	}


	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	aiMatrix4x4 trans = nd->mTransformation;
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++)
	{
		aiVector3D* pPos = &(paiMesh->mVertices[i]);
		aiTransformVecByMatrix4(pPos, &trans);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTangent = &(paiMesh->mTangents[i]);
		const aiVector3D* pBinormal = &(paiMesh->mBitangents[i]);
		const aiVector3D* pTexCoord = &paiMesh->mTextureCoords[0][i];

		Vertex v(
			glm::vec3(pPos->x, pPos->y, pPos->z),
			glm::vec2(pTexCoord->x, pTexCoord->y),
			glm::vec3(pNormal->x, pNormal->y, pNormal->z),
			glm::vec3(pTangent->x, pTangent->y, pTangent->z),
			glm::vec3(pBinormal->x, pBinormal->y, pBinormal->z));

		vertices.push_back(v);

	}

	unsigned int index = 0;
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++)
	{
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		indices.push_back(Face.mIndices[0]);
		indices.push_back(Face.mIndices[1]);
		indices.push_back(Face.mIndices[2]);

		index += 3;
	}


	subset->Setup(vertices, indices);

	boundingbox.CreateFromPoints((glm::vec3*)&vertices[0], vertices.size(), sizeof(Vertex));
	meshSubsets.push_back(subset);

}

void InstanceStaticModelData::Draw(unsigned int numInstances, std::vector<glm::mat4>& positions)
{
	for (GLuint i = 0; i < meshSubsets.size(); i++)
		meshSubsets[i]->Draw(numInstances, positions);
}

const InstancedMeshSubset* InstanceStaticModelData::GetSubset(int index)
{
	return meshSubsets[index];
}

int InstanceStaticModelData::SubsetCount()
{
	return meshSubsets.size();
}

