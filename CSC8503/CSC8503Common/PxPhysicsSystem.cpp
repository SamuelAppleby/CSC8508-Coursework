#include "PxPhysicsSystem.h"
#include <iostream>

PxFilterFlags ContactReportFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, 
	PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize) {
	PX_UNUSED(attributes0);
	PX_UNUSED(attributes1);
	PX_UNUSED(filterData0);
	PX_UNUSED(filterData1);
	PX_UNUSED(constantBlockSize);
	PX_UNUSED(constantBlock);

	// all initial and persisting reports for everything, with per-point data
	pairFlags = PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_TOUCH_FOUND 
		/*| PxPairFlag::eNOTIFY_TOUCH_PERSISTS*/ | PxPairFlag::eNOTIFY_TOUCH_LOST | PxPairFlag::eDETECT_CCD_CONTACT;

	return PxFilterFlags();
}

PxPhysicsSystem::PxPhysicsSystem() {
	realFrames = IDEAL_FRAMES;
	fixedDeltaTime = IDEAL_DT;
	dTOffset = 0.0f;

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc = PxSceneDesc(gPhysics->getTolerancesScale());

	sceneDesc.flags |= PxSceneFlag::eENABLE_CCD;		
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = ContactReportFilterShader;
	sceneDesc.simulationEventCallback = new NCL::CSC8503::GameWorld;
	gScene = gPhysics->createScene(sceneDesc);

	PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
	if (pvdClient) {
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
		pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
	}

	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
}

void PxPhysicsSystem::StepPhysics(float dt) {
	dTOffset += dt;
	while (dTOffset >= fixedDeltaTime) {
		gScene->simulate(fixedDeltaTime);
		gScene->fetchResults(true);
		dTOffset -= fixedDeltaTime;
	}
	NCL::GameTimer t;
	t.Tick();
	float updateTime = t.GetTimeDeltaSeconds();
	if (updateTime > fixedDeltaTime) {
		realFrames /= 2;
		fixedDeltaTime *= 2;
	}
	else if (dt * 2 < fixedDeltaTime) {
		realFrames *= 2;
		fixedDeltaTime /= 2;

		if (realFrames > IDEAL_FRAMES) {
			realFrames = IDEAL_FRAMES;
			fixedDeltaTime = IDEAL_DT;
		}
	}
}

void PxPhysicsSystem::CleanupPhysics() {
	/*PX_RELEASE(gScene);
	PX_RELEASE(gDispatcher);
	PX_RELEASE(gPhysics);*/
	if (gPvd) {
		PxPvdTransport* transport = gPvd->getTransport();
		gPvd->release();
		gPvd = NULL;
		//PX_RELEASE(transport);
	}
	//PX_RELEASE(gFoundation);

	printf("SnippetHelloWorld done.\n");
}



