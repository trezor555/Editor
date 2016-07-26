#include "ModelManager.h"

std::map<std::string, StaticModelData*> ModelManager::m_LoadedStaticModelsMap;

StaticMesh* ModelManager::LoadStaticModel(const char* FilePath)
{
	std::map<std::string, StaticModelData*>::iterator modelIterator;

	modelIterator = m_LoadedStaticModelsMap.find(FilePath);

	if (modelIterator == m_LoadedStaticModelsMap.end())
	{
		StaticModelData* sModel = new StaticModelData();
		if (sModel->Load(FilePath))
		{
			m_LoadedStaticModelsMap.insert(std::pair<std::string, StaticModelData*>(FilePath, sModel));
			StaticMesh* pStaticMesh = new StaticMesh(sModel);
			pStaticMesh->SetFileName(FilePath);
			return pStaticMesh;
		}
	}
	else
	{
		StaticMesh* pStaticMesh = new StaticMesh(modelIterator->second);
		pStaticMesh->SetFileName(FilePath);
		return pStaticMesh;
	}
	
	return nullptr;
}

InstancedStaticMesh* ModelManager::LoadInstancedStaticModel(const char* FilePath)
{
	InstanceStaticModelData* insData = new InstanceStaticModelData();
	if (insData->Load(FilePath))
	{
		InstancedStaticMesh* newInst = new InstancedStaticMesh(insData);
		newInst->FilePath = FilePath;
		return newInst;
	}
	else
	{
		return nullptr;
	}
}

SkinnedMesh* ModelManager::LoadAnimatedMesh(const char* FilePath)
{
	SkinnedMesh* skinned = new SkinnedMesh();
	skinned->filePath = FilePath;
	if (skinned->LoadMesh(FilePath))
	{
		return skinned;
	}
	
	return nullptr;
}
