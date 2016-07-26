#include "SceneManager.h"
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <gtx/rotate_vector.hpp>
unsigned int SceneManager::MID = 0;
unsigned int SceneManager::WAYPOINT_ID = 1;
#include <utility>

unsigned int SceneManager::AddObject(StaticMesh* mesh)
{
	models.insert(std::pair<unsigned int, StaticMesh*>(MID, mesh));
	return MID++;
}

void SceneManager::Draw(IShader& shader)
{
	std::map<unsigned int, StaticMesh*>::iterator iter = models.begin();
	for (iter; iter != models.end(); ++iter)
	{
		BoundingBox transformedBox = iter->second->GetModelData()->boundingbox;
	
		glm::vec3 objRot = iter->second->GetRotation();
		glm::mat4 rotMat;
		objRot = (objRot * glm::pi<float>()) / 180.0f;
		rotMat = glm::rotate(rotMat, objRot.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rotMat = glm::rotate(rotMat, objRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rotMat = glm::rotate(rotMat, objRot.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec4 transformedMaxPoint = rotMat * glm::vec4(transformedBox.max, 0);
		glm::vec4 transformedMinPoint = rotMat * glm::vec4(transformedBox.min, 0);
		transformedBox.max = glm::vec3(transformedMaxPoint.x, transformedMaxPoint.y, transformedMaxPoint.z);
		transformedBox.min = glm::vec3(transformedMinPoint.x, transformedMinPoint.y, transformedMinPoint.z);

		transformedBox.max += iter->second->GetPosition();
		transformedBox.min += iter->second->GetPosition();
		
		//TestResult t = m_Frustum.Intersect(transformedBox);
		//if (t != TEST_OUTSIDE)
		//{
			glm::mat4 worldM;
			glm::vec3 rotation = iter->second->GetRotation();
			glm::vec3 rotationInRadians = (rotation * glm::pi<float>()) / 180.0f;
			worldM = glm::translate(worldM, iter->second->GetPosition());
			worldM = glm::rotate(worldM, rotationInRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
			worldM = glm::rotate(worldM, rotationInRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
			worldM = glm::rotate(worldM, rotationInRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));
			shader.SetWorldMatrix(worldM);
			iter->second->Draw();
		//}
	}
}

void SceneManager::DrawBoundingBoxes(IShader& shader)
{
	std::map<unsigned int, StaticMesh*>::const_iterator iter = models.begin();

	for (iter; iter != models.end(); ++iter)
	{
		if (iter->second->m_bDrawBoundingBox)
		{
			glm::mat4 worldM;
			glm::vec3 rotation = iter->second->GetRotation();
			glm::vec3 rotationInRadians = (rotation * glm::pi<float>()) / 180.0f;
			worldM = glm::translate(worldM, iter->second->GetPosition());
			worldM = glm::rotate(worldM, rotationInRadians.x, glm::vec3(1.0f, 0.0f, 0.0f));
			worldM = glm::rotate(worldM, rotationInRadians.y, glm::vec3(0.0f, 1.0f, 0.0f));
			worldM = glm::rotate(worldM, rotationInRadians.z, glm::vec3(0.0f, 0.0f, 1.0f));
			shader.SetWorldMatrix(worldM);
			iter->second->GetModelData()->boundingbox.Draw();
		}
	}
}

void SceneManager::PickObject(glm::mat4& view, glm::mat4& projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight)
{
	if (selectedStaticMesh != nullptr)
		selectedStaticMesh->m_bDrawBoundingBox = false;

	raypicker.CalculateRays(view, projection, mouseXPos, mouseYPos, screenwidth, screenheight);

	std::map<unsigned int, StaticMesh*>::iterator iter = models.begin();

	bool foundMesh = false;
	float tmin = 99999999;
	glm::vec3 intersec;
	float dist;
	for (iter; iter != models.end(); ++iter)
	{
		BoundingBox transformedBox = iter->second->GetModelData()->boundingbox;
		
		glm::vec3 objRot = iter->second->GetRotation();

		glm::mat4 rotMat;
		objRot = (objRot * glm::pi<float>()) / 180.0f;
		rotMat = glm::rotate(rotMat, objRot.x, glm::vec3(1.0f, 0.0f, 0.0f));
		rotMat = glm::rotate(rotMat, objRot.y, glm::vec3(0.0f, 1.0f, 0.0f));
		rotMat = glm::rotate(rotMat, objRot.z, glm::vec3(0.0f, 0.0f, 1.0f));

		glm::vec4 transformedMaxPoint = rotMat * glm::vec4(transformedBox.max, 0);
		glm::vec4 transformedMinPoint = rotMat * glm::vec4(transformedBox.min, 0);
		transformedBox.max = glm::vec3(transformedMaxPoint.x, transformedMaxPoint.y, transformedMaxPoint.z);
		transformedBox.min = glm::vec3(transformedMinPoint.x, transformedMinPoint.y, transformedMinPoint.z);

		transformedBox.max += iter->second->GetPosition();
		transformedBox.min += iter->second->GetPosition();
		if (raypicker.Intersects(transformedBox, intersec, dist))
		{
			if (dist < tmin)
			{
				foundMesh = true;
				selectedStaticMesh = iter->second;
				m_SelectedObjectID = iter->first;
				tmin = dist;
			}
			
			//SelectedObjects.push_back(iter->second);
		}
	}

	if (!foundMesh)
	{
		if (selectedStaticMesh != nullptr)
			selectedStaticMesh->m_bDrawBoundingBox = false;

		selectedStaticMesh = nullptr;
		m_SelectedObjectID = -1;
	}
	else
	{
		selectedStaticMesh->m_bDrawBoundingBox = true;
	}
}

SceneManager::SceneManager()
{
	selectedStaticMesh = nullptr;
	m_SelectedObjectID = -1;
	m_CurrentInstancedStaticMesh = nullptr;
}

void SceneManager::Init()
{
	StaticMesh* mesh = ModelManager::LoadStaticModel("../Data/Models/Misc/waypoint.X");
	m_miscObjects.insert(std::pair<std::string, StaticMesh*>("waypoint", mesh));
}

void SceneManager::AddAIPAth(AIPath path)
{
	path.SetName("AiPath" + std::to_string(WAYPOINT_ID));
	path.SetID(WAYPOINT_ID);
	m_AIPaths.insert(std::pair<unsigned int, AIPath>(WAYPOINT_ID, path));
	WAYPOINT_ID++;
}

void SceneManager::DrawWaypoints(IShader& shader)
{
	std::map<unsigned int, AIPath>::iterator iter = m_AIPaths.begin();

	for (iter; iter != m_AIPaths.end(); ++iter)
	{
		if (iter->second.IsVisible())
		{
			std::vector<glm::vec3> path = iter->second.GetWaypoints();
			for (int i = 0; i < path.size(); ++i)
			{
				glm::mat4 translation;
				translation = glm::translate(translation, path[i]);
				shader.SetWorldMatrix(translation);
				m_miscObjects["waypoint"]->Draw();

				if (iter->second.IsClosed())
				{
					if (path.size() > 1 && (i - 1) < path.size())
					{
						glBegin(GL_LINES);
						glm::vec3 cpos = path[i - 1];
						glm::vec3 npos = path[i];
						glm::vec3 min = cpos - npos;
						glVertex3f(0, 5, 0);
						glVertex3f(min.x, min.y + 5, min.z);
						glEnd();
					}

					if (path.size() > 2 && (i) == path.size() - 1)
					{
						glBegin(GL_LINES);
						glm::vec3 cpos = path[0];
						glm::vec3 npos = path[path.size() - 1];
						glm::vec3 min = cpos - npos;
						glVertex3f(0, 5, 0);
						glVertex3f(min.x, min.y + 5, min.z);
						glEnd();
					}
				}
				else
				{
					if (path.size() > 1 && (i - 1) < path.size())
					{
						glBegin(GL_LINES);
						glm::vec3 cpos = path[i - 1];
						glm::vec3 npos = path[i];
						glm::vec3 min = cpos - npos;
						glVertex3f(0, 5, 0);
						glVertex3f(min.x, min.y + 5, min.z);
						glEnd();
					}
				}
			}
		}
	}
}

void SceneManager::DrawTempWaypoints(IShader& shader, AIPath& tempPath)
{
	std::vector<glm::vec3> path = tempPath.GetWaypoints();
	for (int i = 0; i < path.size(); ++i)
	{
		glm::mat4 translation;
		translation = glm::translate(translation, path[i]);
		shader.SetWorldMatrix(translation);
		m_miscObjects["waypoint"]->Draw();
		if (tempPath.IsClosed())
		{
			if (path.size() > 1 && (i - 1) < path.size())
			{
				glBegin(GL_LINES);
				glm::vec3 cpos = path[i - 1];
				glm::vec3 npos = path[i];
				glm::vec3 min = cpos - npos;
				glVertex3f(0, 5, 0);
				glVertex3f(min.x, min.y + 5, min.z);
				glEnd();
			}

			if (path.size() > 2 && (i) == path.size()-1)
			{
				glBegin(GL_LINES);
				glm::vec3 cpos = path[0];
				glm::vec3 npos = path[path.size() - 1];
				glm::vec3 min = cpos - npos;
				glVertex3f(0, 5, 0);
				glVertex3f(min.x, min.y + 5, min.z);
				glEnd();
			}
		}
		else
		{
			if (path.size() > 1 && (i - 1) < path.size())
			{
				glBegin(GL_LINES);
				glm::vec3 cpos = path[i - 1];
				glm::vec3 npos = path[i];
				glm::vec3 min = cpos - npos;
				glVertex3f(0, 5, 0);
				glVertex3f(min.x, min.y + 5, min.z);
				glEnd();
			}
		}
	}
}

std::vector<ObjectData> SceneManager::GetAIPath()
{
	std::vector<ObjectData> waypointData;
	std::map<unsigned int, AIPath>::iterator iter = m_AIPaths.begin();

	for (iter; iter != m_AIPaths.end(); ++iter)
	{
		ObjectData data;
		data.id = iter->first;
		data.name = "Waypoint_" + std::to_string(iter->first);
		data.Visible = iter->second.IsVisible();
		waypointData.push_back(data);
	}

	return waypointData;
}

void SceneManager::SetWaypointVisibliy(unsigned int waypointID, bool val)
{
	std::map<unsigned int, AIPath>::iterator iter;
	iter = m_AIPaths.find(waypointID);

	if (iter != m_AIPaths.end())
	{
		iter->second.SetVisible(val);
	}
}

std::vector<ObjectData> SceneManager::GetGameObjectsData()
{
	std::vector<ObjectData> waypointData;
	std::map<unsigned int, StaticMesh*>::iterator iter = models.begin();

	/*for (iter; iter != models.end(); ++iter)
	{
		ObjectData data;
		data.id = iter->first;
		data.name = "Waypoint_" + std::to_string(iter->first);
		data.Visible = iter->second.Visible;
		waypointData.push_back(data);
	}*/

	return waypointData;
}

void SceneManager::ClearScene()
{
	m_AIPaths.clear();

	std::map<unsigned int, StaticMesh*>::iterator iter = models.begin();
	for (iter; iter != models.end(); ++iter)
	{
		delete iter->second;
	}
	models.clear();

	std::map<unsigned int, AnimatedObject>::iterator iter2 = m_animatedObjects.begin();
	for (iter2; iter2 != m_animatedObjects.end(); ++iter2)
	{
		delete iter2->second.animator;
		delete iter2->second.skinnedMesh;
	}
	m_animatedObjects.clear();

	std::map<unsigned int, InstancedStaticMesh*>::iterator iter3 = m_instancedMeshMap.begin();
	for (iter3; iter3 != m_instancedMeshMap.end(); ++iter3)
	{
		delete iter3->second;
	}
	m_instancedMeshMap.clear();

	WAYPOINT_ID = 1;
	selectedStaticMesh = nullptr;
	m_SelectedObjectID = -1;
	m_CurrentInstancedStaticMesh = nullptr;
}

void SceneManager::DeleteSelectedObject()
{
	if (m_SelectedObjectID != -1)
	{
		std::map<unsigned int, StaticMesh*>::iterator iter;
		iter = models.find(m_SelectedObjectID);

		if (iter != models.end())
		{
			delete iter->second;
			models.erase(iter);
			m_SelectedObjectID = -1;
			selectedStaticMesh = nullptr;
		}
	}
}

unsigned int SceneManager::AddInstancedModel(InstancedStaticMesh* instancedMesh)
{
	m_instancedMeshMap.insert(std::pair<unsigned int, InstancedStaticMesh*>(MID, instancedMesh));
	return MID++;
}

void SceneManager::DrawInstancedModels()
{
	std::map<unsigned int, InstancedStaticMesh*>::iterator iter = m_instancedMeshMap.begin();

	for (iter; iter != m_instancedMeshMap.end(); ++iter)
	{
		if (iter->second->BlendEnabled)
		{
			glEnable(GL_BLEND);
			glDepthFunc(GL_LEQUAL);
			glEnable(GL_ALPHA_TEST);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		if (!iter->second->ZWriteEnabled)
		{
			glDepthMask(GL_FALSE);
		}

		if (!iter->second->CullFaceEnabled)
		{
			glDisable(GL_CULL_FACE);
		}

		iter->second->Draw();

		if (iter->second->BlendEnabled)
		{
			glDisable(GL_BLEND);
			glBlendFunc(GL_ONE, GL_ZERO);
		}

		if (!iter->second->ZWriteEnabled)
		{
			glDepthMask(GL_TRUE);
		}

		if (!iter->second->CullFaceEnabled)
		{
			glEnable(GL_CULL_FACE);
		}

	}
}

void SceneManager::SelectInstancedStaticMesh(unsigned int index)
{
	std::map<unsigned int, InstancedStaticMesh*>::iterator iter = m_instancedMeshMap.begin();
	iter = m_instancedMeshMap.find(index);

	if (iter != m_instancedMeshMap.end())
	{
		m_CurrentInstancedStaticMesh = iter->second;
	}
	else
	{
		m_CurrentInstancedStaticMesh = nullptr;
	}
}

InstancedStaticMesh* SceneManager::GetCurrentInstancedStaticMesh()
{
	return m_CurrentInstancedStaticMesh;
}

std::vector<ModelData> SceneManager::GetStaticModelData()
{
	std::vector<ModelData> mData;
	std::map<unsigned int, StaticMesh*>::iterator iter = models.begin();
	for (iter; iter != models.end(); ++iter)
	{
		ModelData dat;
		dat.path = iter->second->GetFileName();
		dat.pos = iter->second->GetPosition();
		dat.rot = iter->second->GetRotation();
		mData.push_back(dat);
	}

	return mData;
}

std::vector<InstancedStaticMesh*> SceneManager::GetInstancedMeshes()
{
	std::vector<InstancedStaticMesh*> models;
	std::map<unsigned int, InstancedStaticMesh*>::iterator iter = m_instancedMeshMap.begin();

	if (iter != m_instancedMeshMap.end())
	{
		models.push_back(iter->second);
	}

	return models;
}

void SceneManager::AddAnimatedmesh(SkinnedMesh* skinnedMesh)
{
	if (skinnedMesh != nullptr)
	{
		SceneAnimator* scAnimator = new SceneAnimator(skinnedMesh->m_pScene);
		AnimatedObject animObject;
		animObject.animator = scAnimator;
		animObject.skinnedMesh = skinnedMesh;
		m_animatedObjects.insert(std::pair<unsigned int, AnimatedObject>(MID, animObject));
		MID++;
	}
}

void SceneManager::DrawAnimatedModels(float deltaTime, float timeInSeconds, SkinnedMeshShader* skinnedMeshShader, glm::mat4& view, glm::mat4& projection)
{
	std::map<unsigned int, AnimatedObject>::iterator iter = m_animatedObjects.begin();
	
	glm::mat4 projView = projection * view;
	for (iter; iter != m_animatedObjects.end(); ++iter)
	{
		//BoundingSphere sph = iter->second.skinnedMesh->bsphere;
		//sph.center += iter->second.skinnedMesh->GetPositon();
		//if (m_Frustum.Intersect(sph) != TEST_OUTSIDE)
		//{
			
			vector<glm::mat4> Transforms;
			iter->second.skinnedMesh->Update(deltaTime);
			//iter->second.animator->Calculate(timeInSeconds*0.001);
			//Transforms = iter->second.animator->GetBoneMatrices(iter->second.skinnedMesh->m_pScene->mRootNode->mChildren[0]);
			iter->second.skinnedMesh->BoneTransform(timeInSeconds, Transforms);

			//skinnedmesh->BoneTransform(deltaTime, Transforms);
			aiMatrix4x4 aiMe = iter->second.animator->GetGlobalTransform(iter->second.skinnedMesh->m_pScene->mRootNode->mChildren[0]);
			

			glm::mat4 SkinnedWorldM;
		
			glm::vec3 currentPosition = iter->second.skinnedMesh->GetPositon();
			currentPosition.y = m_Terrain->GetHeight(currentPosition.x, currentPosition.z);

			SkinnedWorldM = glm::translate(SkinnedWorldM, currentPosition);
			glm::mat4 rot = glm::rotate(iter->second.skinnedMesh->rotation.y, glm::vec3(0, 1, 0));

			skinnedMeshShader->SetWorldMatrix(SkinnedWorldM);
			skinnedMeshShader->SetWVP(projView * SkinnedWorldM * rot);
			for (unsigned int i = 0; i < Transforms.size(); i++)
			{
				skinnedMeshShader->SetBoneTransform(i, Transforms[i]);
			}

			iter->second.skinnedMesh->Render();
		//}
		
	}
}

void SceneManager::SetFrustum(Frustum& frustum)
{
	m_Frustum = frustum;
}

float IntersectSphere(RayPicker raypicker, BoundingSphere sph)
{
	glm::vec3 Q = sph.center - raypicker.GetRay().origin;
	double c = glm::length(Q);
	double v = glm::dot(Q, raypicker.GetRay().direction);
	double d = sph.radius*sph.radius - (c*c - v*v);

	// If there was no intersection, return -1
	if (d < 0.0) 
		return (-1.0f);

	// Return the distance to the [first] intersecting point
	return (v - sqrt(d));
}

float SceneManager::intersectRaySphere(glm::dmat4 view, glm::dmat4 projection, int mouseXPos, int mouseYPos, int screenwidth, int screenheight)
{
	raypicker.CalculateRays(view, projection, mouseXPos, mouseYPos, screenwidth, screenheight);
	std::map<unsigned int, AnimatedObject>::iterator iter = m_animatedObjects.begin();
	float ret = -1;
	for (iter; iter != m_animatedObjects.end(); ++iter)
	{
		BoundingSphere sph = iter->second.skinnedMesh->bsphere;
		sph.center += iter->second.skinnedMesh->GetPositon();
		if (IntersectSphere(raypicker, sph) != -1)
		{
			selectedAnimObject = iter->second;
			ret = 1;
		}
	}

	return ret;
}

const AIPath& SceneManager::GetPath(int index)
{
	std::map<unsigned int, AIPath>::iterator iter;

	iter = m_AIPaths.find(index);

	if (iter != m_AIPaths.end())
		return iter->second;
}

std::vector<SkinnedMesh*> SceneManager::GetSkinnedMeshes()
{
	std::map<unsigned int, AnimatedObject>::iterator iter = m_animatedObjects.begin();
	std::vector<SkinnedMesh*> ret;
	for (iter; iter != m_animatedObjects.end(); ++iter)
		ret.push_back(iter->second.skinnedMesh);

	return ret;
}

std::vector<AIPath> SceneManager::GetAiPaths()
{
	std::vector<AIPath> paths;
	std::map<unsigned int, AIPath>::iterator iter = m_AIPaths.begin();

	for (iter; iter != m_AIPaths.end(); ++iter)
		paths.push_back(iter->second);

	return paths;
}

void SceneManager::SetTerrain(TerrainV2* terrain)
{
	m_Terrain = terrain;
}

void SceneManager::AddAIPathFromFile(AIPath& aipath)
{
	aipath.SetName("AiPath" + std::to_string(aipath.GetID()));
	m_AIPaths.insert(std::pair<unsigned int, AIPath>(aipath.GetID(), aipath));
	WAYPOINT_ID = aipath.GetID();
	WAYPOINT_ID++;
}



