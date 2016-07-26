#include "StaticMesh.h"
#include <glm.hpp>

unsigned int StaticMesh::GUID = 0;

void StaticMesh::Draw() const
{
	m_pModelData->Draw();
}

void StaticMesh::SetPosition(const glm::vec3& Positon)
{
	m_Position = Positon;
}

const glm::vec3& StaticMesh::GetPosition() const
{
	return m_Position;
}

StaticMesh::StaticMesh(StaticModelData* modelData) 
{
	m_pModelData = modelData;
	m_bDrawBoundingBox = false;
	ID = GUID++;
}

StaticModelData* const StaticMesh::GetModelData() const
{
	return m_pModelData;
}

void StaticMesh::Clear()
{
}

std::string StaticMesh::GetFileName() const
{
	return m_szFileName;
}

void StaticMesh::SetFileName(std::string fileName)
{
	m_szFileName = fileName;
}

unsigned int StaticMesh::GetId() const
{
	return ID;
}

const glm::vec3& StaticMesh::GetRotation() const
{
	return m_Rotation;
}

void StaticMesh::AddRotationX(float angle)
{
	m_Rotation.x += angle;
	if (m_Rotation.x > 360)
	{
		m_Rotation.x = 0;
	}
}

void StaticMesh::AddRotationY(float angle)
{
	m_Rotation.y += angle;
	if (m_Rotation.y > 360)
	{
		m_Rotation.y = 0;
	}
}

void StaticMesh::AddRotationZ(float angle)
{
	m_Rotation.z += angle;
	if (m_Rotation.z > 360)
	{
		m_Rotation.z = 0;
	}
}

void StaticMesh::SetRotationX(float angle)
{
	m_Rotation.x = angle;
}

void StaticMesh::SetRotationY(float angle)
{
	m_Rotation.y = angle;
}

void StaticMesh::SetRotationZ(float angle)
{
	m_Rotation.z = angle;
}

void StaticMesh::SetPositionX(float val)
{
	m_Position.x = val;
}

void StaticMesh::SetPositionY(float val)
{
	m_Position.y = val;
}

void StaticMesh::SetPositionZ(float val)
{
	m_Position.z = val;
}

void StaticMesh::SetRotation(const glm::vec3& rotation)
{
	m_Rotation = rotation;
}
