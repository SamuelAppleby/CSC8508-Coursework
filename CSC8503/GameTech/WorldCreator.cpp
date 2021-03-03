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

OGLShader* WorldCreator::basicShader = nullptr;

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

void WorldCreator::AddPxCubeToWorld(const PxTransform& t, const Vector3 halfSizes) {
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *pXPhysics->GetGMaterial());
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* cube = new GameObject;
	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), body));
	world->AddGameObject(cube);
}

void WorldCreator::AddPxSphereToWorld(const PxTransform& t, const  PxReal radius) {
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *pXPhysics->GetGMaterial());
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* sphere = new GameObject;
	sphere->GetTransform().SetScale(Vector3(radius, radius, radius));
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), body));
	world->AddGameObject(sphere);
}

void WorldCreator::AddPxCapsuleToWorld(const PxTransform& t, const  PxReal radius, const PxReal halfHeight) {
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, halfHeight), 
		*pXPhysics->GetGMaterial())->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 10);
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* capsule = new GameObject;
	capsule->GetTransform().SetScale(Vector3(radius * 2, halfHeight * 2, radius * 2));
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), body));
	world->AddGameObject(capsule);
}

void WorldCreator::AddPxFloorToWorld(const PxTransform& t, const Vector3 halfSizes) {
	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *pXPhysics->GetGMaterial());
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* floor = new GameObject;
	floor->GetTransform().SetScale(halfSizes * 2);
	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), body));
	world->AddGameObject(floor);
}

void WorldCreator::AddPxPickupToWorld(const PxTransform& t, const PxReal radius) {
	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t.transform(PxTransform(t.p)));;
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *pXPhysics->GetGMaterial());
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* p = new GameObject;
	p->GetTransform().SetScale(Vector3(radius, radius, radius));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), bonusMesh, basicTex, basicShader));
	p->GetRenderObject()->SetColour(Debug::YELLOW);
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), body));
	world->AddGameObject(p);
}

void WorldCreator::AddPxPlayerToWorld(const PxTransform& t, const PxReal scale) {
	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(meshSize * .85f, meshSize * 0.85f),
		*pXPhysics->GetGMaterial())->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* p = new GameObject;
	p->GetTransform().SetScale(Vector3(meshSize * 2, meshSize * 2, meshSize * 2));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), charMeshA, basicTex, basicShader));
	p->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
	p->SetPhysicsObject(new PhysicsObject(&p->GetTransform(), body));
	world->AddGameObject(p);
}

void WorldCreator::AddPxEnemyToWorld(const PxTransform& t, const PxReal scale) {
	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(meshSize * .85f, meshSize * 0.85f),
		*pXPhysics->GetGMaterial())->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, 10.0f);
	pXPhysics->GetGScene()->addActor(*body);

	GameObject* e = new GameObject;
	e->GetTransform().SetScale(Vector3(meshSize * 2, meshSize * 2, meshSize * 2));
	e->SetRenderObject(new RenderObject(&e->GetTransform(), enemyMesh, basicTex, basicShader));
	e->GetRenderObject()->SetColour(Vector4(1, 0, 0, 1));
	e->SetPhysicsObject(new PhysicsObject(&e->GetTransform(), body));
	world->AddGameObject(e);
}