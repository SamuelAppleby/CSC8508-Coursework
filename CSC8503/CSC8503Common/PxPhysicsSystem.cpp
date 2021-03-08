#include "PxPhysicsSystem.h"
#include <iostream>

PxPhysicsSystem::PxPhysicsSystem() {
	realFrames = IDEAL_FRAMES;
	fixedDeltaTime = IDEAL_DT;
	dTOffset = 0.0f;

	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);

	gPvd = PxCreatePvd(*gFoundation);
	PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
	gPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f * GRAVITY_SCALE, 0.0f);
	gDispatcher = PxDefaultCpuDispatcherCreate(2);
	sceneDesc.cpuDispatcher = gDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
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
