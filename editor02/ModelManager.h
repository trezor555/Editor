#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <map>
#include <string>
#include "StaticMesh.h"
#include "InstancedStaticMesh.h"
#include "SkinnedMesh.h"


class ModelManager
{
public:
	static StaticMesh* LoadStaticModel(const char* FilePath);
	static InstancedStaticMesh* LoadInstancedStaticModel(const char* FilePath);
	static SkinnedMesh* LoadAnimatedMesh(const char* FilePath);
private:
	static std::map<std::string, StaticModelData*> m_LoadedStaticModelsMap;
	//static std::map<std::string, SkinnedMeshda>
};

#endif