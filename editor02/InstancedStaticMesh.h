#ifndef INSTANCED_STATIC_MESH_H
#define INSTANCED_STATIC_MESH_H
#include "InstancedStaticModelData.h"

class InstancedStaticMesh
{
public:
	InstancedStaticMesh(InstanceStaticModelData* modelData);
	void Clear();
	void Draw();
	void SetPosition(glm::vec3 Positon);
	glm::vec3 GetPosition() const;
	InstanceStaticModelData* GetModelData();
	void AddInstance(glm::vec3& pos, float angle);
	bool DrawBoundingBox;

	bool BlendEnabled;
	bool ZWriteEnabled;
	bool CullFaceEnabled;
	struct Pos_Rot
	{
		Pos_Rot(glm::vec3 pos, float angle)
		{
			position = pos; this->angle = angle;
		}
		glm::vec3 position;
		float angle;
	};

	std::string FilePath;
	std::vector<Pos_Rot> m_PositionsRotations;
	std::vector<glm::mat4> m_TransformationMatrices;
	unsigned int m_numInstance;
private:
	glm::vec3 m_Position;
	InstanceStaticModelData* m_modelData;

	
	
	
};

#endif