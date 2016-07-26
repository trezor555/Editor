#include "SkinnedMesh.h"
#include <gtx/quaternion.hpp>
#include <Windows.h>
#define POSITION_LOCATION    0
#define TEX_COORD_LOCATION   1
#define NORMAL_LOCATION      2
#define BONE_ID_LOCATION     3
#define BONE_WEIGHT_LOCATION 4
#define ARRAY_SIZE_IN_ELEMENTS(a) (sizeof(a)/sizeof(a[0]))
inline glm::mat4 aiMatrix4x4ToGlm(aiMatrix4x4* from)
{
	glm::mat4 to;


	to[0][0] = (GLfloat)from->a1; to[0][1] = (GLfloat)from->b1;  to[0][2] = (GLfloat)from->c1; to[0][3] = (GLfloat)from->d1;
	to[1][0] = (GLfloat)from->a2; to[1][1] = (GLfloat)from->b2;  to[1][2] = (GLfloat)from->c2; to[1][3] = (GLfloat)from->d2;
	to[2][0] = (GLfloat)from->a3; to[2][1] = (GLfloat)from->b3;  to[2][2] = (GLfloat)from->c3; to[2][3] = (GLfloat)from->d3;
	to[3][0] = (GLfloat)from->a4; to[3][1] = (GLfloat)from->b4;  to[3][2] = (GLfloat)from->c4; to[3][3] = (GLfloat)from->d4;

	return to;
}

void SkinnedMesh::VertexBoneData::AddBoneData(unsigned int BoneID, float Weight)
{
	for (unsigned int i = 0; i < ARRAY_SIZE_IN_ELEMENTS(IDs); i++) 
	{
		if (Weights[i] == 0.0) 
		{
			IDs[i] = BoneID;
			Weights[i] = Weight;
			return;
		}
	}

	// should never get here - more bones than we have space for
	//assert(0);
}

SkinnedMesh::SkinnedMesh()
{
	m_VAO = 0;
	m_Buffers;
	m_NumBones = 0;
	m_pScene = nullptr;


	_animationTime0 = 0.0f;
	_animationTime1 = 0.0f;


	_curScene = 0;
	_prevAnimIndex = 1;

	m_lastTime = -1;
	

	
	_blendingTime = 0.0f;
	_blendingTimeMul=1.0f;
	_updateBoth = true;
	_temporary=false;
	_playTime=0.0f;
}


SkinnedMesh::~SkinnedMesh()
{
	Clear();
}


void SkinnedMesh::Clear()
{
	if (m_Buffers[0] != 0) {
		glDeleteBuffers(ARRAY_SIZE_IN_ELEMENTS(m_Buffers), m_Buffers);
	}

	if (m_VAO != 0) {
		glDeleteVertexArrays(1, &m_VAO);
		m_VAO = 0;
	}
}


bool SkinnedMesh::LoadMesh(const char* Filename)
{
	// Release the previously loaded mesh (if it exists)
	//Clear();

	// Create the VAO
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// Create the buffers for the vertices attributes
	glGenBuffers(NUM_VBs, m_Buffers);

	bool Ret = false;

	m_pScene = m_Importer.ReadFile(Filename,  aiProcess_GenSmoothNormals);

	if (m_pScene) 
	{
		Ret = InitFromScene(m_pScene, Filename);
	}
	else {
		printf("Error parsing '%s': '%s'\n", Filename, m_Importer.GetErrorString());
	}

	// Make sure the VAO is not changed from the outside
	glBindVertexArray(0);

	return Ret;
}


bool SkinnedMesh::InitFromScene(const aiScene* pScene, const string& Filename)
{
	
	
	m_Entries.resize(pScene->mNumMeshes);
	//m_Textures.resize(pScene->mNumMaterials);

	vector<glm::vec3> Positions;
	vector<glm::vec3> Normals;
	vector<glm::vec2> TexCoords;
	vector<VertexBoneData> Bones;
	vector<unsigned int> Indices;

	unsigned int NumVertices = 0;
	unsigned int NumIndices = 0;

	// Count the number of vertices and indices
	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		m_Entries[i].MaterialIndex = pScene->mMeshes[i]->mMaterialIndex;
		m_Entries[i].NumIndices = pScene->mMeshes[i]->mNumFaces * 3;
		m_Entries[i].BaseVertex = NumVertices;
		m_Entries[i].BaseIndex = NumIndices;

		NumVertices += pScene->mMeshes[i]->mNumVertices;
		NumIndices += m_Entries[i].NumIndices;
	}

	// Reserve space in the vectors for the vertex attributes and indices
	Positions.reserve(NumVertices);
	Normals.reserve(NumVertices);
	TexCoords.reserve(NumVertices);
	Bones.resize(NumVertices);
	Indices.reserve(NumIndices);
	m_GlobalInverseTransform = pScene->mRootNode->mTransformation;
	// Initialize the meshes in the scene one by one
	for (unsigned int i = 0; i < m_Entries.size(); i++)
	{
		const aiMesh* paiMesh = pScene->mMeshes[i];
		InitMesh(&m_Entries[i], pScene, i, paiMesh, Positions, Normals, TexCoords, Bones, Indices);
	}
	for (int i = 0; i < pScene->mNumAnimations; ++i)
	{
		m_AnimationMap.insert(std::pair<std::string, unsigned int>(pScene->mAnimations[i]->mName.C_Str(), i));
	}
	

	bsphere.radius = max((bbox.max.x - bbox.min.x) / 2, (bbox.max.y - bbox.min.y) / 2, (bbox.max.z - bbox.min.z) / 2);
	bsphere.center = glm::vec3(bbox.min.x + (bbox.max.x - bbox.min.x) / 2, bbox.min.y + (bbox.max.y - bbox.min.y) / 2, bbox.min.z + (bbox.max.z - bbox.min.z) / 2);

	if (!InitMaterials(pScene, Filename)) 
	{
		return false;
	}

	// Generate and populate the buffers with vertex attributes and the indices
	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[POS_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Positions[0]) * Positions.size(), &Positions[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(POSITION_LOCATION);
	glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[TEXCOORD_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TexCoords[0]) * TexCoords.size(), &TexCoords[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(TEX_COORD_LOCATION);
	glVertexAttribPointer(TEX_COORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Normals[0]) * Normals.size(), &Normals[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(NORMAL_LOCATION);
	glVertexAttribPointer(NORMAL_LOCATION, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, m_Buffers[BONE_VB]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(BONE_ID_LOCATION);
	glVertexAttribIPointer(BONE_ID_LOCATION, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
	glEnableVertexAttribArray(BONE_WEIGHT_LOCATION);
	glVertexAttribPointer(BONE_WEIGHT_LOCATION, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Buffers[INDEX_BUFFER]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices[0]) * Indices.size(), &Indices[0], GL_STATIC_DRAW);

	return true;
}


void SkinnedMesh::InitMesh(MeshEntry* meshentry, const aiScene* pScene, unsigned int MeshIndex,
	const aiMesh* paiMesh,
	vector<glm::vec3>& Positions,
	vector<glm::vec3>& Normals,
	vector<glm::vec2>& TexCoords,
	vector<VertexBoneData>& Bones,
	vector<unsigned int>& Indices)
{
	const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
	
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

				Texture tex;
				tex.textureID = tex2d->GetID();
				tex.textureType = TEXTURE_DIFFUSE;
				meshentry->m_textures.push_back(tex);
			}
		}
	}

	// Populate the vertex attribute vectors
	for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) 
	{
		const aiVector3D* pPos = &(paiMesh->mVertices[i]);
		const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
		const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;

		Positions.push_back(glm::vec3(pPos->x, pPos->y, pPos->z));
		Normals.push_back(glm::vec3(pNormal->x, pNormal->y, pNormal->z));
		TexCoords.push_back(glm::vec2(pTexCoord->x, pTexCoord->y));
	}

	bbox.CreateFromPoints((glm::vec3*)&Positions[0], Positions.size(), sizeof(glm::vec3));
	
	LoadBones(MeshIndex, paiMesh, Bones);

	// Populate the index buffer
	for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) 
	{
		const aiFace& Face = paiMesh->mFaces[i];
		assert(Face.mNumIndices == 3);
		Indices.push_back(Face.mIndices[0]);
		Indices.push_back(Face.mIndices[1]);
		Indices.push_back(Face.mIndices[2]);
	}
}


void SkinnedMesh::LoadBones(unsigned int MeshIndex, const aiMesh* pMesh, vector<VertexBoneData>& Bones)
{
	for (unsigned int i = 0; i < pMesh->mNumBones; i++) 
	{
		unsigned int BoneIndex = 0;
		string BoneName(pMesh->mBones[i]->mName.data);

		if (m_BoneMapping.find(BoneName) == m_BoneMapping.end()) 
		{
			// Allocate an index for a new bone
			BoneIndex = m_NumBones;
			m_NumBones++;
			BoneInfo bi;
			m_BoneInfo.push_back(bi);
			m_BoneInfo[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
			m_BoneMapping[BoneName] = BoneIndex;
		}
		else 
		{
			BoneIndex = m_BoneMapping[BoneName];
		}

		for (unsigned int j = 0; j < pMesh->mBones[i]->mNumWeights; j++)
		{
			unsigned int VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
			float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
			Bones[VertexID].AddBoneData(BoneIndex, Weight);
		}
	}
}


bool SkinnedMesh::InitMaterials(const aiScene* pScene, const string& Filename)
{
	
	return true;
}


void SkinnedMesh::Render()
{
	glBindVertexArray(m_VAO);

	for (unsigned int i = 0; i < m_Entries.size(); i++) 
	{
		if (m_Entries[i].m_textures.size() > 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_Entries[i].m_textures[i].textureID);
		}

		glDrawElementsBaseVertex(GL_TRIANGLES,
			m_Entries[i].NumIndices,
			GL_UNSIGNED_INT,
			(void*)(sizeof(unsigned int)* m_Entries[i].BaseIndex),
			m_Entries[i].BaseVertex);
	}

	// Make sure the VAO is not changed from the outside    
	glBindVertexArray(0);
}

void SkinnedMesh::SetPosition(glm::vec3& position)
{
	m_Position = position;
}

glm::vec3& SkinnedMesh::GetPositon()
{
	return m_Position;
}

void SkinnedMesh::SetRotation(glm::vec3& rotation)
{
	m_Rotation = rotation;
}

glm::vec3& SkinnedMesh::GetRotation()
{
	return m_Rotation;
}

void InitM4FromM3(aiMatrix4x4& out, const aiMatrix3x3& in)
{
	out.a1 = in.a1; out.a2 = in.a2; out.a3 = in.a3; out.a4 = 0.f;
	out.b1 = in.b1; out.b2 = in.b2; out.b3 = in.b3; out.b4 = 0.f;
	out.c1 = in.c1; out.c2 = in.c2; out.c3 = in.c3; out.c4 = 0.f;
	out.d1 = 0.f;   out.d2 = 0.f;   out.d3 = 0.f;   out.d4 = 1.f;
}

void InitIdentityM4(aiMatrix4x4 &m)
{
	m.a1 = 1.f; m.a2 = 0.f; m.a3 = 0.f; m.a4 = 0.f;
	m.b1 = 0.f; m.b2 = 1.f; m.b3 = 0.f; m.b4 = 0.f;
	m.c1 = 0.f; m.c2 = 0.f; m.c3 = 1.f; m.c4 = 0.f;
	m.d1 = 0.f; m.d2 = 0.f; m.d3 = 0.f; m.d4 = 1.f;
	assert(m.IsIdentity());
}

void MulM4(aiMatrix4x4 &out, aiMatrix4x4 &in, float m)
{
	out.a1 += in.a1 * m; out.a2 += in.a2 * m; out.a3 += in.a3 * m; out.a4 += in.a4 * m;
	out.b1 += in.b1 * m; out.b2 += in.b2 * m; out.b3 += in.b3 * m; out.b4 += in.b4 * m;
	out.c1 += in.c1 * m; out.c2 += in.c2 * m; out.c3 += in.c3 * m; out.c4 += in.c4 * m;
	out.d1 += in.d1 * m; out.d2 += in.d2 * m; out.d3 += in.d3 * m; out.d4 += in.d4 * m;
}

void ShortMulM4(aiVector3D &out, const aiMatrix4x4 &m, const aiVector3D &in)
{
	out.x = m.a1 * in.x + m.a2 * in.y + m.a3 * in.z;
	out.y = m.b1 * in.x + m.b2 * in.y + m.b3 * in.z;
	out.z = m.c1 * in.x + m.c2 * in.y + m.c3 * in.z;
}

void SkinnedMesh::BoneTransform(float TimeInSeconds, vector<glm::mat4>& Transforms)
{
	aiMatrix4x4 Identity;
	InitIdentityM4(Identity);
	
	
	
	if (_blendingTime > 0.0f)
	{
		float TicksPerSecond = m_pScene->mAnimations[_prevAnimIndex]->mTicksPerSecond != 0 ? m_pScene->mAnimations[_prevAnimIndex]->mTicksPerSecond : 25.0f;
		float TimeInTicks = _animationTime0 * TicksPerSecond;
		float AnimationTime0 = fmod(TimeInTicks, m_pScene->mAnimations[_prevAnimIndex]->mDuration);

		TicksPerSecond = m_pScene->mAnimations[_curScene]->mTicksPerSecond != 0 ?
			m_pScene->mAnimations[_curScene]->mTicksPerSecond : 25.0f;
		TimeInTicks = _animationTime1 * TicksPerSecond;
		float AnimationTime1 = fmod(TimeInTicks, m_pScene->mAnimations[_curScene]->mDuration);

		ReadNodeHeirarchy(m_pScene->mAnimations[_prevAnimIndex], m_pScene->mAnimations[_curScene]
			, AnimationTime0, AnimationTime1
			, m_pScene->mRootNode, m_pScene->mRootNode
			, Identity, 1);
	}
	else
	{

		float TicksPerSecond = m_pScene->mAnimations[_curScene]->mTicksPerSecond != 0 ? m_pScene->mAnimations[_curScene]->mTicksPerSecond : 25.0f;
		float TimeInTicks = _animationTime0* TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, m_pScene->mAnimations[_curScene]->mDuration);

		ReadNodeHeirarchy(m_pScene->mAnimations[_curScene], AnimationTime, m_pScene->mRootNode, Identity, 1);
	}

	Transforms.resize(m_NumBones);

	for (unsigned int i = 0; i < m_NumBones; i++) 
	{
		Transforms[i] = aiMatrix4x4ToGlm(&m_BoneInfo[i].FinalTransformation);
	}
}

void SkinnedMesh::ReadNodeHeirarchy(const aiAnimation * scene0, const aiAnimation * scene1
	, float AnimationTime0, float AnimationTime1
	, const aiNode* pNode0, const aiNode* pNode1
	, const aiMatrix4x4& ParentTransform, int stopAnimLevel)
{
	float time0(AnimationTime0);
	float time1(AnimationTime1);

	std::string NodeName0(pNode0->mName.data);
	std::string NodeName1(pNode1->mName.data);
	assert(NodeName0 == NodeName1);

	const aiAnimation* pAnimation0 = scene0;
	const aiAnimation* pAnimation1 = scene1;

	aiMatrix4x4 NodeTransformation0(pNode0->mTransformation);
	aiMatrix4x4 NodeTransformation1(pNode1->mTransformation);

	const aiNodeAnim* pNodeAnim0 = FindNodeAnim(pAnimation0, NodeName0);
	const aiNodeAnim* pNodeAnim1 = FindNodeAnim(pAnimation1, NodeName0);

	if (pNodeAnim0 && pNodeAnim1) 
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling0;
		CalcInterpolatedScaling(Scaling0, time0, pNodeAnim0);

		aiVector3D Scaling1;
		CalcInterpolatedScaling(Scaling1, time1, pNodeAnim1);

		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling0 * _blendingTime + Scaling1 * (1.f - _blendingTime), ScalingM);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ0;
		CalcInterpolatedRotation(RotationQ0, time0, pNodeAnim0);
		aiQuaternion RotationQ1;
		CalcInterpolatedRotation(RotationQ1, time1, pNodeAnim1);
		aiMatrix4x4 RotationM;
		aiQuaternion RotationQ;
		aiQuaternion::Interpolate(RotationQ, RotationQ1, RotationQ0, _blendingTime);
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation0;
		{
			//float time(stopAnimLevel <= 0 ? AnimationTime0 : 0.f);
			CalcInterpolatedPosition(Translation0, AnimationTime0, pNodeAnim0);
		}
		aiVector3D Translation1;
		{
			//float time(stopAnimLevel <= 0 ? AnimationTime1 : 0.f);
			CalcInterpolatedPosition(Translation1, AnimationTime1, pNodeAnim1);
		}
		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation0 * _blendingTime + Translation1 * (1.f - _blendingTime), TranslationM);

		// Combine the above transformations
		NodeTransformation0 = TranslationM * RotationM * ScalingM;
	}

	stopAnimLevel--;

	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation0;

	if (m_BoneMapping.find(NodeName0) != m_BoneMapping.end())
	{
		unsigned int BoneIndex = m_BoneMapping[NodeName0];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
	}

	unsigned int n = min(pNode0->mNumChildren, pNode1->mNumChildren);
	for (unsigned int i = 0; i < n; i++) 
	{
		ReadNodeHeirarchy(scene0, scene1, AnimationTime0, AnimationTime1, pNode0->mChildren[i], pNode1->mChildren[i], GlobalTransformation, stopAnimLevel);
	}
}

void SkinnedMesh::ReadNodeHeirarchy(const aiAnimation * scene, float AnimationTime, const aiNode* pNode, const aiMatrix4x4& ParentTransform, int stopAnimLevel)
{
	float time(AnimationTime);

	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = scene;

	aiMatrix4x4 NodeTransformation(pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) 
	{
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, time, pNodeAnim);
		aiMatrix4x4 ScalingM;
		aiMatrix4x4::Scaling(Scaling, ScalingM);

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, time, pNodeAnim);
		aiMatrix4x4 RotationM;
		InitM4FromM3(RotationM, RotationQ.GetMatrix());

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		{
			//float time(stopAnimLevel <= 0 ? AnimationTime : 0.f);
			CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
		}
		aiMatrix4x4 TranslationM;
		aiMatrix4x4::Translation(Translation, TranslationM);

		// Combine the above transformations
		NodeTransformation = TranslationM * RotationM * ScalingM;
	}


	aiMatrix4x4 GlobalTransformation = ParentTransform * NodeTransformation;

	if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) 
	{
		unsigned int BoneIndex = m_BoneMapping[NodeName];
		m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation * m_BoneInfo[BoneIndex].BoneOffset;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) 
	{
		ReadNodeHeirarchy(scene, AnimationTime, pNode->mChildren[i], GlobalTransformation, stopAnimLevel);
	}
}


unsigned int SkinnedMesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
	{
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
		{
			return i;
		}
	}

	//assert(0);
	return 0xFFFFFFFF;
}

void SkinnedMesh::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) 
	{
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	//assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}

unsigned int SkinnedMesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//assert(pNodeAnim->mNumScalingKeys > 0);


	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) 
		{
			return i;
		}
	}

	assert(0);
	return 0xFFFFFFFF;
}

void SkinnedMesh::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumScalingKeys == 1) 
	{
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	//assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartScaling = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& EndScaling = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	Out = StartScaling * (1 - Factor) + EndScaling * Factor;
}

unsigned int SkinnedMesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	//assert(pNodeAnim->mNumPositionKeys > 0);
	
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) 
	{
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) 
		{
			return i;
		}
	}

	assert(0);
	return 0xFFFFFFFF;
}

void SkinnedMesh::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumPositionKeys == 1) 
	{
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	//assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime;
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;
	//assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& StartPosition = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& EndPosition = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	Out = StartPosition * (1 - Factor) + EndPosition * Factor;
}

const aiNodeAnim* SkinnedMesh::FindNodeAnim(const aiAnimation* pAnimation, const std::string &nodeName)
{
	for (unsigned int i = 0; i < pAnimation->mNumChannels; ++i)
	{
		if (strcmp(pAnimation->mChannels[i]->mNodeName.C_Str(), nodeName.c_str()) == 0)
		{
			return pAnimation->mChannels[i];
		}
	}
	return nullptr;
}

void SkinnedMesh::Update(float dt1)
{
	if (aipath.GetWaypoints().size() > 0)
	{
		std::vector<glm::vec3> waypoints = aipath.GetWaypoints();
		glm::vec3 toNextWaypoint;
		currentWaypoint = waypoints[currentWaypointIndex];
		if (currentWaypointIndex == waypoints.size() - 1)
		{
			nextWaypoint = waypoints[0];
			rotationAlpha = 1.0f;
		}
		else
		{
			nextWaypoint = waypoints[currentWaypointIndex + 1];
			rotationAlpha = 1.0f;
		}

		toNextWaypoint = nextWaypoint - currentWaypoint;

		float toNextWaypointLength = glm::length(toNextWaypoint);
		glm::vec3 currentPos;
		if (alpha > 0.0f)
		{
			alpha -= (180.0f / toNextWaypointLength)*dt1*0.5;
			currentPos = alpha * currentWaypoint + (1.0f - alpha) * nextWaypoint;
			m_Position = currentPos;
		}
		else
		{
			alpha = 1.0f;
			currentWaypointIndex += 1;
		}

		if (currentWaypointIndex == waypoints.size())
		{
			currentWaypointIndex = 0;
			rotationAlpha = 1.0f;
		}


		float fObjectYRotation = atan2(nextWaypoint.x - currentWaypoint.x, nextWaypoint.z - currentWaypoint.z);
		float toRotate = rotation.y - fObjectYRotation;

		if (rotationAlpha > 0)
		{
			rotationAlpha -= dt1 * 5;
		}

		rotation.y = rotationAlpha * rotation.y + (1.0f - rotationAlpha) * fObjectYRotation;
	}

	if (m_lastTime == -1)
	{
		m_lastTime = GetTickCount();
	}
	long long newTime = GetTickCount();
	float dt = (float)((double)newTime - (double)m_lastTime) / 1000.0f;
	m_lastTime = newTime;

	
	_animationTime0 += dt;
	_animationTime1 += dt;

	if (_blendingTime >= 0)
	{
		_blendingTime -= 1.8f * dt;
	}

	/*if (_blendingTime > 0.0f)
	{
		_blendingTime -= dt * _blendingTimeMul;
		if (_blendingTime <= 0.0f)
		{
			_animationTime0 = _animationTime1;
		}
		if (_updateBoth)
		{
			_animationTime1 += dt;
		}
	}
	else
	{
		_animationTime1 += dt;
	}*/

	/*if (_temporary)
	{
		_playTime -= dt1;
		if (_playTime <= 0.f)
		{
			_temporary = false;
			SetAnimIndex(_prevAnimIndex);
		}
	}*/
}

bool  SkinnedMesh::SetAnimIndex(std::string animationName)
{
	if (_curScene == m_AnimationMap[animationName])
	{
		return false;
	}
	
	_prevAnimIndex = _curScene;
	_curScene = m_AnimationMap[animationName];
	_blendingTime = 1.0f;
	//_blendingTimeMul = 1.0f / 1.0f;
	//_animationTime1 = 0.0f;
	//_updateBoth = updateBoth;
	//_temporary = temporary;
	//_playTime = time;
	return true;
}
