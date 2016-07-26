#ifndef SCREENMANAGER_H
#define SCREENMANAGER_H

#include "StaticMesh.h"
#include "IShader.h"
#include "RayPicker.h"
#include "ModelManager.h"
#include "SkinnedMesh.h"
#include "SkinnedMeshAnimator.h"
#include "SkinnedMeshShader.h"
#include "AIPath.h"
#include "VertexDataTypes.h"
#include "InstancedStaticMesh.h"
#include "Frustum.h"
#include "TerrainV2.h"
#include <map>

class SceneManager
{
public:
	SceneManager();
	void Init();
	unsigned int AddObject(StaticMesh* mesh);
	void Draw(IShader& shader);
	void DrawBoundingBoxes(IShader& shader);
	void PickObject(glm::mat4& view, glm::mat4& projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight);
	void DrawWaypoints(IShader& shader);
	void DrawTempWaypoints(IShader& shader, AIPath& tempPath);
	void AddAIPAth(AIPath path);
	void SetWaypointVisibliy(unsigned int waypointID, bool val);
	unsigned int AddInstancedModel(InstancedStaticMesh* instancedMesh);
	void DrawInstancedModels();
	
	void DeleteSelectedObject();
	std::vector<ObjectData> GetAIPath();
	std::vector<ObjectData> GetGameObjectsData();


	std::map<unsigned int, StaticMesh*> models;
	StaticMesh* dropInMesh;
	RayPicker raypicker;
	std::vector<StaticMesh*> SelectedObjects;
	std::map<std::string, StaticMesh*> m_miscObjects;
	StaticMesh* selectedStaticMesh;

	void ClearScene();
	void SelectInstancedStaticMesh(unsigned int index);
	InstancedStaticMesh* GetCurrentInstancedStaticMesh();
	std::vector<ModelData> GetStaticModelData();
	std::vector<InstancedStaticMesh*> GetInstancedMeshes();
	std::vector<AIPath> GetAiPaths();
	struct AnimatedObject 
	{
		SceneAnimator* animator;
		SkinnedMesh* skinnedMesh;
	};

	void AddAnimatedmesh(SkinnedMesh* skinnedMesh);
	void DrawAnimatedModels(float deltaTime, float timeInSeconds, SkinnedMeshShader* skinnedMeshShader, glm::mat4& view, glm::mat4& projection);
	void SetFrustum(Frustum& frustum);
	int drawednimatedModels = 0;
	void RayVsSphere();
	float intersectRaySphere(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight);
	AnimatedObject selectedAnimObject;
	const AIPath& SceneManager::GetPath(int index);
	std::map<unsigned int, AnimatedObject> m_animatedObjects;
	std::vector<SkinnedMesh*> GetSkinnedMeshes();
	void SetTerrain(TerrainV2* terrain);
	void AddAIPathFromFile(AIPath& aipath);
	unsigned int VisibleStatiMeshes;
private:
	TerrainV2* m_Terrain;
	int m_SelectedObjectID;
	static unsigned int MID;
	static unsigned int WAYPOINT_ID;
	std::map<unsigned int, AIPath> m_AIPaths;
	std::map<unsigned int, InstancedStaticMesh*> m_instancedMeshMap;
	
	InstancedStaticMesh* m_CurrentInstancedStaticMesh;
	Frustum m_Frustum;
};

#endif