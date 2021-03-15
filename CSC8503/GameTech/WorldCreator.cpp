#include "WorldCreator.h"
PxPhysicsSystem* WorldCreator::pXPhysics = nullptr;
GameWorld* WorldCreator::world = nullptr;

OGLMesh* WorldCreator::charMeshA = nullptr;
OGLMesh* WorldCreator::charMeshB = nullptr;
OGLMesh* WorldCreator::enemyMesh = nullptr;
OGLMesh* WorldCreator::bonusMesh = nullptr;
OGLMesh* WorldCreator::capsuleMesh = nullptr;
OGLMesh* WorldCreator::cubeMesh = nullptr;
OGLMesh* WorldCreator::sphereMesh = nullptr;

OGLTexture* WorldCreator::basicTex = nullptr;
OGLTexture* WorldCreator::floorTex = nullptr;
OGLTexture* WorldCreator::lavaTex = nullptr;
OGLTexture* WorldCreator::iceTex = nullptr;
OGLTexture* WorldCreator::trampolineTex = nullptr;
OGLTexture* WorldCreator::obstacleTex = nullptr;
OGLTexture* WorldCreator::woodenTex = nullptr;
OGLTexture* WorldCreator::finishTex = nullptr;
OGLTexture* WorldCreator::menuTex = nullptr;
OGLTexture* WorldCreator::plainTex = nullptr;
OGLTexture* WorldCreator::wallTex = nullptr;
OGLTexture* WorldCreator::dogeTex = nullptr;

OGLShader* WorldCreator::basicShader = nullptr;
OGLShader* WorldCreator::toonShader = nullptr;

void WorldCreator::Create(PxPhysicsSystem* p, GameWorld* w) {
	pXPhysics = p;
	world = w;
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh", &cubeMesh);
	loadFunc("sphere.msh", &sphereMesh);
	loadFunc("Male1.msh", &charMeshA);
	loadFunc("courier.msh", &charMeshB);
	loadFunc("security.msh", &enemyMesh);
	loadFunc("coin.msh", &bonusMesh);
	loadFunc("capsule.msh", &capsuleMesh);

	basicTex = (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	obstacleTex = (OGLTexture*)TextureLoader::LoadAPITexture("obstacle.png");
	floorTex = (OGLTexture*)TextureLoader::LoadAPITexture("platform.png");
	lavaTex = (OGLTexture*)TextureLoader::LoadAPITexture("lava.png");
	trampolineTex = (OGLTexture*)TextureLoader::LoadAPITexture("trampoline.png");
	iceTex = (OGLTexture*)TextureLoader::LoadAPITexture("ice.png");
	woodenTex = (OGLTexture*)TextureLoader::LoadAPITexture("wood.png");
	finishTex = (OGLTexture*)TextureLoader::LoadAPITexture("finish.png");
	menuTex = (OGLTexture*)TextureLoader::LoadAPITexture("menu.png");
	plainTex = (OGLTexture*)TextureLoader::LoadAPITexture("plain.png");
	wallTex = (OGLTexture*)TextureLoader::LoadAPITexture("wall.png");
	dogeTex = (OGLTexture*)TextureLoader::LoadAPITexture("doge.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
	toonShader = new OGLShader("ToonShaderVertex.glsl", "ToonShaderFragment.glsl");
}

WorldCreator::~WorldCreator() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;
	delete basicTex;
	delete obstacleTex;
	delete floorTex;
	delete lavaTex;
	delete trampolineTex;
	delete iceTex;
	delete woodenTex;
	delete finishTex;
	delete menuTex;
	delete basicShader;
}

void WorldCreator::AddPxCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, float density, float friction, float elasticity) {
	GameObject* cube = new GameObject("Cube");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cube->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, toonShader));
	world->AddGameObject(cube);
}

void WorldCreator::AddPxSphereToWorld(const PxTransform& t, const  PxReal radius, float density, float friction, float elasticity) {
	GameObject* sphere = new GameObject("Sphere");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	sphere->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	sphere->GetTransform().SetScale(PxVec3(radius, radius, radius));
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, toonShader));
	world->AddGameObject(sphere);
}

void WorldCreator::AddPxCapsuleToWorld(const PxTransform& t, const  PxReal radius, const PxReal halfHeight, float density, float friction, float elasticity) {
	GameObject* capsule = new GameObject("Capsule");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, halfHeight), *newMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	capsule->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	capsule->GetTransform().SetScale(PxVec3(radius * 2, halfHeight * 2, radius * 2));
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, toonShader));
	world->AddGameObject(capsule);
}

void WorldCreator::AddPxFloorToWorld(const PxTransform& t, const PxVec3 halfSizes, float friction, float elasticity) {
	GameObject* floor = new GameObject("Floor");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	floor->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	floor->GetTransform().SetScale(halfSizes * 2);
	if(!friction)
		floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, iceTex, toonShader));
	else
		floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, toonShader));
	world->AddGameObject(floor);
}

void WorldCreator::AddPxPickupToWorld(const PxTransform& t, const PxReal radius) {
	GameObject* p = new GameObject("Pickup");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));;
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *pXPhysics->GetGMaterial());
	p->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	p->GetTransform().SetScale(PxVec3(radius, radius, radius));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), bonusMesh, basicTex, toonShader));
	p->GetRenderObject()->SetColour(Debug::YELLOW);
	world->AddGameObject(p);
}

void WorldCreator::AddPxPlayerToWorld(const PxTransform& t, const PxReal scale) {
	GameObject* p = new GameObject("Player");
	
	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(meshSize * .85f, meshSize * 0.85f),
	*pXPhysics->GetGMaterial())->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 40.0f);
	p->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	body->setMaxLinearVelocity(50);
	pXPhysics->GetGScene()->addActor(*body);

	p->GetTransform().SetScale(PxVec3(meshSize * 2, meshSize * 2, meshSize * 2));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshA, basicTex, toonShader));
	p->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
	world->AddGameObject(p);
}

void WorldCreator::AddPxEnemyToWorld(const PxTransform& t, const PxReal scale) {
	GameObject* e = new GameObject("Enemy");

	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(meshSize * .85f, meshSize * 0.85f),
		*pXPhysics->GetGMaterial())->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	e->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	e->GetTransform().SetScale(PxVec3(meshSize * 2, meshSize * 2, meshSize * 2));
	e->SetRenderObject(new RenderObject(&e->GetTransform(), enemyMesh, basicTex, toonShader));
	e->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	world->AddGameObject(e);
}

void WorldCreator::AddPxSeeSawToWorld(const PxTransform & t, const PxVec3 halfSizes, float density, float friction, float elasticity) {
	GameObject* cube = new GameObject("SeeSaw");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cube->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);
	PxRevoluteJoint* joint = PxRevoluteJointCreate(*pXPhysics->GetGPhysics(), body, PxTransform(PxVec3(0)), NULL, PxTransform(t.p * 2));

	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, toonShader));
	world->AddGameObject(cube);
}

void WorldCreator::AddPxRevolvingDoorToWorld(const PxTransform& t, const PxVec3 halfSizes, float density, float friction, float elasticity) {
	GameObject* cube = new GameObject("RevolvingDoor");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cube->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);
	PxD6Joint* joint = PxD6JointCreate(*pXPhysics->GetGPhysics(), body, PxTransform(PxVec3(0)), NULL, PxTransform(t.p * 2));
	joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);

	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, toonShader));
	world->AddGameObject(cube);
}

void WorldCreator::AddPxRotatingCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 rotation, float friction, float elasticity) {
	GameObject* cube = new GameObject("RotatingCube");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setAngularVelocity(rotation);
	body->setAngularDamping(0.f);
	body->setMass(0.f);
	body->setMassSpaceInertiaTensor(PxVec3(0.f));
	cube->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, toonShader));
	world->AddGameObject(cube);
}

void WorldCreator::AddPxCannonBallToWorld(const PxTransform& t, Cannon* cannonObj, const  PxReal radius, const PxVec3* force, float density, float friction, float elasticity) {
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	pXPhysics->GetGScene()->addActor(*body);
	Cannonball* sphere = new Cannonball(body);
	sphere->GetTransform().SetScale(PxVec3(radius, radius, radius));
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, toonShader));
	sphere->SetPhysicsObject(new PhysXObject(body, newMat));
	world->AddGameObject(sphere);
	body->addForce(*force, PxForceMode::eIMPULSE);
	cannonObj->addShot(sphere);
}

void WorldCreator::AddPxCannonToWorld(const PxTransform& t, const PxVec3 trajectory, const int shotTime, const int shotSize) {
	Cannon* cannon = new Cannon(trajectory, shotTime, shotSize);

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(30, 15, 15), *pXPhysics->GetGMaterial());
	cannon->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	cannon->GetTransform().SetScale(PxVec3(30, 15, 15) * 2);
	cannon->SetRenderObject(new RenderObject(&cannon->GetTransform(), cubeMesh, obstacleTex, toonShader));
	world->AddGameObject(cannon);
}

void WorldCreator::AddPxKillPlaneToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 respawnCentre, Vector3 respawnSizeRange, bool hide) {
	KillPlane* cube = new KillPlane(respawnCentre, respawnSizeRange);

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *pXPhysics->GetGMaterial());
	cube->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	cube->GetTransform().SetScale(halfSizes * 2);
	if (!hide) {
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, lavaTex, basicShader));
	}
	world->AddGameObject(cube);
}