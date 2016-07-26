#ifndef COLLISION_H
#define COLLISION_H
#include <glm.hpp>
#include <map>
#include "PxPhysicsAPI.h"

using namespace physx;

class PhysXCollision
{
public:
	void Initialize();
	void Release();
	void Update(float dt);
	void CookMesh(void* verts, void* indices, PxU32 vertCount, PxU32 indexCount, const glm::vec3& pos, unsigned int id, const glm::vec3& rotation);
	void CookTerrain(unsigned short* heightfield, int width, int depth);
	void ReCookTerrain(unsigned short* heightfield, int width, int depth, int cellWidth);
	void ChangeRigidBodyPos(unsigned int id, const glm::vec3& newPos);
	void ChangeRigidBodyRot(unsigned int id, const glm::vec3& newPos);
	void DeleteMesh(unsigned int id);
	PxController* characterController;
private:

	float mAccumulator;
	float mStepSize;
	PxMaterial* mMaterial;
	PxRigidStatic * terrainActor;
	PxScene* mScene;
	PxFoundation* mFoundation;
	PxDefaultCpuDispatcher* mCpuDispatcher;
	PxCooking* mCooking;
	PxPhysics* mPhysicsSDK;
	PxDefaultErrorCallback gDefaultErrorCallback;
	PxDefaultAllocator gDefaultAllocatorCallback;
	PxSimulationFilterShader gDefaultFilterShader;

	PxControllerManager* controllerManager;

	PxHeightField* heightField;
	PxHeightFieldSample* samples;
	PxShape* aHeightFieldShape;
	std::map<unsigned int, PxRigidStatic*> m_RigidBodyMap;
};

#endif
