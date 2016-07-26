#include "PhysXCollision.h"
#include <glm.hpp>

void PhysXCollision::Initialize()
{
	mAccumulator = 0.0f;
	mStepSize = 1.0f / 120.0f;

	mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback, gDefaultErrorCallback);
	if (!mFoundation)
		return;

	bool recordMemoryAllocations = true;
	mPhysicsSDK = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation,
		PxTolerancesScale(), recordMemoryAllocations, nullptr);
	if (!mPhysicsSDK)
		return;

	mCooking = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation, PxCookingParams(PxTolerancesScale()));
	if (!mCooking)
		return;


	PxSceneDesc sceneDesc(mPhysicsSDK->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;

	mScene = mPhysicsSDK->createScene(sceneDesc);
	if (!mScene)
		return;

	controllerManager = PxCreateControllerManager(*mScene);
	mMaterial = mPhysicsSDK->createMaterial(0.5, 0.5, 0.5);
	PxCapsuleControllerDesc desc;
	desc.position = PxExtendedVec3(1500.0f, 1500.0f, 1500.0f);
	desc.contactOffset = 0.1f;
	desc.stepOffset = 0.5f;
	//desc.climbingMode = PxCapsuleClimbingMode::eEASY;
	//desc.nonWalkableMode = PxControllerNonWalkableMode::;
	
	desc.slopeLimit = glm::radians(40.0f);
	
	desc.radius = 10.0f;
	desc.height = 40.0f;
	desc.upDirection = PxVec3(0, 1, 0);
	desc.material = mMaterial;

	characterController = controllerManager->createController(desc);
}

void PhysXCollision::Release()
{
	mScene->release();
	mPhysicsSDK->release();
	mFoundation->release();
}

void PhysXCollision::Update(float dt)
{
	mScene->simulate(mStepSize);
	mScene->fetchResults(true);
}

void PhysXCollision::CookMesh(void* verts, void* indices, PxU32 vertCount, PxU32 indexCount, const glm::vec3& pos, unsigned int id, const glm::vec3& rotation)
{
	PxTolerancesScale scale;
	PxCookingParams params(scale);
	// disable mesh cleaning - perform mesh validation on development configurations
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
	// disable edge precompute, edges are set for each triangle, slows contact generation
	params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;
	// lower hierarchy for internal mesh
	params.meshCookingHint = PxMeshCookingHint::eCOOKING_PERFORMANCE;

	mCooking->setParams(params);

	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = vertCount;
	meshDesc.points.stride = sizeof(PxVec3);
	meshDesc.points.data = verts;

	meshDesc.triangles.count = indexCount / 3;
	meshDesc.triangles.stride = 3 * sizeof(PxU32);
	meshDesc.triangles.data = indices;

	PxTriangleMesh* aTriangleMesh = mCooking->createTriangleMesh(meshDesc, mPhysicsSDK->getPhysicsInsertionCallback());


	PxMaterial* mmMaterial;

	mmMaterial = mPhysicsSDK->createMaterial(0.0f, 0.0f, 0.0f);
	/*PxTransform planePos = PxTransform(PxVec3(pos.x, pos.y,
		pos.z), PxQuat(PxHalfPi, PxVec3(0.0f, 1.0f, 0.0f)));*/

	PxQuat pX(glm::radians(rotation.x), PxVec3(1, 0, 0));
	PxQuat pY(glm::radians(rotation.y), PxVec3(0, 1, 0));
	PxQuat pZ(glm::radians(rotation.z), PxVec3(0, 0, 1));

	PxQuat p = pX * pY * pZ;

	PxRigidStatic * aSphereActor = mPhysicsSDK->createRigidStatic(PxTransform(PxVec3(pos.x, pos.y,
		pos.z), p));
	PxShape* aTriMeshShape = aSphereActor->createShape(PxTriangleMeshGeometry(aTriangleMesh), *mmMaterial);
	
	mScene->addActor(*aSphereActor);
	m_RigidBodyMap.insert(std::pair<unsigned int, PxRigidStatic*>(id, aSphereActor));

}

void PhysXCollision::CookTerrain(unsigned short* heightfield, int width, int depth)
{
	const PxReal heightScale = 1.0f;
	PxU32 hfNumVerts = width * depth;

	samples = new PxHeightFieldSample[hfNumVerts];
	memset(samples, 0, hfNumVerts*sizeof(PxHeightFieldSample));

	for (int z = 0; z < hfNumVerts; z++)
	{
		samples[z].height = heightfield[z] - 32768;
		samples[z].materialIndex0 = 0;
		samples[z].materialIndex1 = 0;
	}
	
	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = depth;
	hfDesc.nbRows = depth;
	hfDesc.thickness = -10;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	heightField = mCooking->createHeightField(hfDesc, mPhysicsSDK->getPhysicsInsertionCallback());

	
	PxTransform pose(PxVec3(0, 0, 0), PxQuat(PxIdentity));
	terrainActor = mPhysicsSDK->createRigidStatic(pose);

	
	mMaterial = mPhysicsSDK->createMaterial(0.0f, 0.0f, 0.0f);

	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, 32, 32);
	
	aHeightFieldShape = terrainActor->createShape(hfGeom, *mMaterial);


	mScene->addActor(*terrainActor);
	
}

void PhysXCollision::ReCookTerrain(unsigned short* heightfield, int width, int depth, int cellWidth)
{
	mScene->removeActor(*terrainActor);
	delete[] samples;
	aHeightFieldShape->release();
	heightField->release();
	const PxReal heightScale = 1.0f;
	PxU32 hfNumVerts = width * depth;
	


	samples = new PxHeightFieldSample[hfNumVerts];
	memset(samples, 0, hfNumVerts*sizeof(PxHeightFieldSample));

	for (int z = 0; z < hfNumVerts; z++)
	{
		samples[z].height = heightfield[z] - 32768;
		samples[z].materialIndex0 = 0;
		samples[z].materialIndex1 = 0;

	}

	PxHeightFieldDesc hfDesc;
	hfDesc.format = PxHeightFieldFormat::eS16_TM;
	hfDesc.nbColumns = depth;
	hfDesc.nbRows = depth;
	hfDesc.thickness = -10;
	hfDesc.samples.data = samples;
	hfDesc.samples.stride = sizeof(PxHeightFieldSample);

	heightField = mCooking->createHeightField(hfDesc, mPhysicsSDK->getPhysicsInsertionCallback());


	PxTransform pose(PxVec3(0, 0, 0), PxQuat(PxIdentity));
	terrainActor = mPhysicsSDK->createRigidStatic(pose);


	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), heightScale, cellWidth, cellWidth);

	aHeightFieldShape = terrainActor->createShape(hfGeom, *mMaterial);
	mScene->addActor(*terrainActor);
}

void PhysXCollision::ChangeRigidBodyPos(unsigned int id, const glm::vec3& newPos)
{
	PxTransform pose = m_RigidBodyMap[id]->getGlobalPose();
	pose.p = PxVec3(newPos.x, newPos.y, newPos.z);
	m_RigidBodyMap[id]->setGlobalPose(pose);
}

void PhysXCollision::ChangeRigidBodyRot(unsigned int id, const glm::vec3& rotation)
{
	PxTransform pose = m_RigidBodyMap[id]->getGlobalPose();

	PxQuat pX(glm::radians(rotation.x), PxVec3(1, 0, 0));
	PxQuat pY(glm::radians(rotation.y), PxVec3(0, 1, 0));
	PxQuat pZ(glm::radians(rotation.z), PxVec3(0, 0, 1));

	pose.q = pX * pY * pZ;
	m_RigidBodyMap[id]->setGlobalPose(pose);
}

void PhysXCollision::DeleteMesh(unsigned int id)
{
	std::map<unsigned int, PxRigidStatic*>::iterator iter;
	iter = m_RigidBodyMap.find(id);

	if (iter != m_RigidBodyMap.end())
	{
		iter->second->release();
		m_RigidBodyMap.erase(iter);
	}
}
