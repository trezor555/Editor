#include "InstancedStaticMesh.h"

#include <glm.hpp>

void InstancedStaticMesh::Draw()
{
	if (m_numInstance != 0)
	{
		m_modelData->Draw(m_numInstance, m_TransformationMatrices);
	}
}

void InstancedStaticMesh::SetPosition(glm::vec3 Positon)
{
	m_Position = Positon;
}


glm::vec3 InstancedStaticMesh::GetPosition() const
{
	return m_Position;
}

InstancedStaticMesh::InstancedStaticMesh(InstanceStaticModelData* modelData)
{
	m_modelData = modelData;
	DrawBoundingBox = false;
	m_numInstance = 0;

	BlendEnabled = false;
	ZWriteEnabled = true;
	CullFaceEnabled = false;
}

InstanceStaticModelData* InstancedStaticMesh::GetModelData()
{
	return m_modelData;
}

void InstancedStaticMesh::AddInstance(glm::vec3& pos, float angle)
{
	m_PositionsRotations.push_back(Pos_Rot(pos, angle));

	glm::mat4 WorldMat;
	WorldMat = glm::translate(WorldMat, pos);
	WorldMat = glm::rotate(WorldMat, angle, glm::vec3(0, 1, 0));
	m_TransformationMatrices.push_back(WorldMat);
	m_numInstance++;
}
