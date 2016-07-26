#ifndef SKINNED_MESH_H
#define SKINNED_MESH_H
#define GLEW_STATIC
#include <GL/glew.h>

#include <vector>
#include <map>
using namespace std;
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include "BoundingBox.h"
#include "BoundingSphere.h"
#include "AIPath.h"
#include "TextureManager.h"
class SkinnedMesh
{
public:
	AIPath aipath;
	SkinnedMesh();
	~SkinnedMesh();
	bool LoadMesh(const char* Filename);
	void Render();

	unsigned int NumBones() const
	{
		return m_NumBones;
	}

	void BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms);
	const aiScene* m_pScene;

	void SetPosition(glm::vec3& position);
	glm::vec3& GetPositon();

	void SetRotation(glm::vec3& rotation);
	glm::vec3& GetRotation();

	void Update(float dt1);
	bool SetAnimIndex(std::string animationName);
	long long m_lastTime;
	float _animationTime0;
	float _animationTime1;
	float _blendingTime;
	float _blendingTimeMul;
	unsigned int _prevAnimIndex;
	bool _updateBoth;
	bool _temporary;
	float _playTime;
	unsigned int _curScene;

	aiMatrix4x4 m_GlobalInverseTransform;
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string &nodeName);

	void SkinnedMesh::ReadNodeHeirarchy(const aiAnimation * scene0, const aiAnimation * scene1
		, float AnimationTime0, float AnimationTime1
		, const aiNode* pNode0, const aiNode* pNode1
		, const aiMatrix4x4& ParentTransform, int stopAnimLevel);

	void ReadNodeHeirarchy(const aiAnimation * scene0, float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel);
	BoundingBox bbox;
	BoundingSphere bsphere;
	glm::vec3 currentWaypoint;
	glm::vec3 nextWaypoint;
	int currentWaypointIndex = 0;
	float alpha = 1.0f;
	float rotationAlpha = 1.0f;
	glm::vec3 rotation;

	std::string filePath;
	int aipathID = -1;
private:
	std::string m_CurrentAnimation;
	glm::vec3 m_Position;
	glm::vec3 m_Rotation;
	const aiAnimation* mAnim;
	std::map<std::string, unsigned int> m_AnimationMap;

	/** At which frame the last evaluation happened for each channel.
	* Useful to quickly find the corresponding frame for slightly increased time stamps
	*/
	

#define NUM_BONES_PER_VEREX 4

	struct BoneInfo
	{
		aiMatrix4x4 BoneOffset;
		aiMatrix4x4 FinalTransformation;

		BoneInfo()
		{
			
		}
	};

	struct VertexBoneData
	{
		unsigned int IDs[NUM_BONES_PER_VEREX];
		float Weights[NUM_BONES_PER_VEREX];

		VertexBoneData()
		{
			memset(IDs, 0, sizeof(IDs));
			memset(Weights, 0, sizeof(Weights));
			
		};



		void AddBoneData(unsigned int BoneID, float Weight);
	};

#define INVALID_MATERIAL 0xFFFFFFFF

	enum VB_TYPES {
		INDEX_BUFFER,
		POS_VB,
		NORMAL_VB,
		TEXCOORD_VB,
		BONE_VB,
		NUM_VBs
	};


	struct MeshEntry
	{
		MeshEntry()
		{
			NumIndices = 0;
			BaseVertex = 0;
			BaseIndex = 0;
			MaterialIndex = INVALID_MATERIAL;
		}

		unsigned int NumIndices;
		unsigned int BaseVertex;
		unsigned int BaseIndex;
		unsigned int MaterialIndex;
		std::vector<Texture> m_textures;
	};

	
	bool InitFromScene(const aiScene* pScene, const string& Filename);
	void InitMesh(MeshEntry* meshentry, const aiScene* pScene, unsigned int MeshIndex,
		const aiMesh* paiMesh,
		vector<glm::vec3>& Positions,
		vector<glm::vec3>& Normals,
		vector<glm::vec2>& TexCoords,
		vector<VertexBoneData>& Bones,
		vector<unsigned int>& Indices);
	void LoadBones(unsigned int MeshIndex, const aiMesh* paiMesh, vector<VertexBoneData>& Bones);
	bool InitMaterials(const aiScene* pScene, const string& Filename);
	void Clear();


	GLuint m_VAO;
	GLuint m_Buffers[NUM_VBs];

	

	vector<MeshEntry> m_Entries;
	//vector<Texture*> m_Textures;

	map<string, unsigned int> m_BoneMapping; // maps a bone name to its index
	unsigned int m_NumBones;
	vector<BoneInfo> m_BoneInfo;

	Assimp::Importer m_Importer;
};

#endif