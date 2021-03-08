#include "WorldCreator.h"
PxPhysicsSystem* WorldCreator::pXPhysics = nullptr;
PxMaterial* WorldCreator::normalMat = nullptr;
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

OGLShader* WorldCreator::basicShader = nullptr;

void WorldCreator::Create(PxPhysicsSystem* p, GameWorld* w) {
	pXPhysics = p;
	normalMat = pXPhysics->GetGPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
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
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");
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
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
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
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
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
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	world->AddGameObject(capsule);
}

void WorldCreator::AddPxFloorToWorld(const PxTransform& t, const PxVec3 halfSizes) {
	GameObject* floor = new GameObject("Floor");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *normalMat);
	floor->SetPhysicsObject(new PhysXObject(body, normalMat));
	pXPhysics->GetGScene()->addActor(*body);

	floor->GetTransform().SetScale(halfSizes * 2);
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, basicShader));
	world->AddGameObject(floor);
}

void WorldCreator::AddPxPickupToWorld(const PxTransform& t, const PxReal radius) {
	GameObject* p = new GameObject("Pickup");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));;
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *normalMat);
	p->SetPhysicsObject(new PhysXObject(body, normalMat));
	pXPhysics->GetGScene()->addActor(*body);

	p->GetTransform().SetScale(PxVec3(radius, radius, radius));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), bonusMesh, basicTex, basicShader));
	p->GetRenderObject()->SetColour(Debug::YELLOW);
	world->AddGameObject(p);
}

void WorldCreator::AddPxPlayerToWorld(const PxTransform& t, const PxReal scale) {
	GameObject* p = new GameObject("Player");

	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(meshSize * .85f, meshSize * 0.85f),
		*normalMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	p->SetPhysicsObject(new PhysXObject(body, normalMat));
	pXPhysics->GetGScene()->addActor(*body);

	p->GetTransform().SetScale(PxVec3(meshSize * 2, meshSize * 2, meshSize * 2));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshA, basicTex, basicShader));
	p->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
	world->AddGameObject(p);
}

void WorldCreator::AddPxEnemyToWorld(const PxTransform& t, const PxReal scale) {
	GameObject* e = new GameObject("Enemy");

	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(meshSize * .85f, meshSize * 0.85f),
		*normalMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	e->SetPhysicsObject(new PhysXObject(body, normalMat));
	pXPhysics->GetGScene()->addActor(*body);

	e->GetTransform().SetScale(PxVec3(meshSize * 2, meshSize * 2, meshSize * 2));
	e->SetRenderObject(new RenderObject(&e->GetTransform(), enemyMesh, basicTex, basicShader));
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
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
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
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	world->AddGameObject(cube);
}