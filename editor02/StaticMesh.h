#ifndef STATIC_MESH_H
#define STATIC_MESH_H

#include "StaticModelData.h"

class StaticMesh
{
public:
	StaticMesh(StaticModelData* modelData);
	void Clear();
	void Draw() const;
	void SetPosition(const glm::vec3& Positon);

	void AddRotationX(float angle);
	void AddRotationY(float angle);
	void AddRotationZ(float angle);

	void SetRotationX(float angle);
	void SetRotationY(float angle);
	void SetRotationZ(float angle);

	void SetRotation(const glm::vec3& rotation);

	void SetPositionX(float val);
	void SetPositionY(float val);
	void SetPositionZ(float val);

	void SetFileName(std::string fileName);
	std::string GetFileName() const;
	const glm::vec3& GetPosition() const;
	const glm::vec3& GetRotation() const;
	
	StaticModelData* const GetModelData() const;
	bool m_bDrawBoundingBox;
	unsigned int GetId() const;

private:
	unsigned int ID;
	std::string m_szFileName;
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	StaticModelData* m_pModelData;
	static unsigned int GUID;
};

#endif