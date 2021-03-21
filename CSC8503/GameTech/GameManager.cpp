#include "GameManager.h"
Win32Code::Win32Window* GameManager::window = nullptr;

PxPhysicsSystem* GameManager::pXPhysics = nullptr;
GameWorld* GameManager::world = nullptr;
GameTechRenderer* GameManager::renderer = nullptr;
AudioManager* GameManager::audioManager = nullptr;
Obstacles* GameManager::obstacles = nullptr;


OGLMesh* GameManager::charMeshA = nullptr;
OGLMesh* GameManager::charMeshB = nullptr;
OGLMesh* GameManager::enemyMesh = nullptr;
OGLMesh* GameManager::bonusMesh = nullptr;
OGLMesh* GameManager::capsuleMesh = nullptr;
OGLMesh* GameManager::cylinderMesh = nullptr;
OGLMesh* GameManager::cubeMesh = nullptr;
OGLMesh* GameManager::sphereMesh = nullptr;

OGLTexture* GameManager::basicTex = nullptr;
OGLTexture* GameManager::floorTex = nullptr;
OGLTexture* GameManager::lavaTex = nullptr;
OGLTexture* GameManager::iceTex = nullptr;
OGLTexture* GameManager::trampolineTex = nullptr;
OGLTexture* GameManager::obstacleTex = nullptr;
OGLTexture* GameManager::woodenTex = nullptr;
OGLTexture* GameManager::finishTex = nullptr;
OGLTexture* GameManager::menuTex = nullptr;
OGLTexture* GameManager::plainTex = nullptr;
OGLTexture* GameManager::wallTex = nullptr;
OGLTexture* GameManager::dogeTex = nullptr;

OGLShader* GameManager::basicShader = nullptr;
OGLShader* GameManager::toonShader = nullptr;

GameObject* GameManager::lockedObject = nullptr;
GameObject* GameManager::selectionObject = nullptr;

LevelState GameManager::levelState = LevelState::LEVEL1;

void GameManager::Create(PxPhysicsSystem* p, GameWorld* w, AudioManager* a)
{
	pXPhysics = p;
	world = w;
	renderer = new GameTechRenderer(*GameManager::GetWorld(), pXPhysics);
	Debug::SetRenderer(renderer);
	renderer->InitGUI(window->GetHandle());
	audioManager = a;
	obstacles = new Obstacles();
}



void GameManager::LoadAssets()
{
	auto loadFunc = [](const string& name, OGLMesh** into)
	{
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
	loadFunc("cylinder.msh", &cylinderMesh);

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

void GameManager::ResetMenu()
{
	levelState = LevelState::LEVEL1;
	selectionObject = nullptr;
	lockedObject = nullptr;
	renderer->SetSelectionObject(nullptr);
	renderer->SetLockedObject(nullptr);
}

GameManager::~GameManager()
{
	delete pXPhysics;
	delete world;
	delete renderer;
	delete audioManager;

	delete capsuleMesh;
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
	delete plainTex;
	delete wallTex;
	delete dogeTex;

	delete basicShader;
	delete toonShader;
}

void GameManager::AddPxCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, float density, float friction, float elasticity)
{
	GameObject* cube = new GameObject("Cube");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cube->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, toonShader));
	world->AddGameObject(cube);
}

void GameManager::AddPxSphereToWorld(const PxTransform& t, const  PxReal radius, float density, float friction, float elasticity)
{
	GameObject* sphere = new GameObject("Sphere");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	sphere->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	sphere->GetTransform().SetScale(PxVec3(radius, radius, radius));
	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, toonShader));
	world->AddGameObject(sphere);
}

void GameManager::AddPxCapsuleToWorld(const PxTransform& t, const  PxReal radius, const PxReal halfHeight, float density, float friction, float elasticity)
{
	GameObject* capsule = new GameObject("Capsule");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, halfHeight), *newMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	capsule->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	capsule->GetTransform().SetScale(PxVec3(radius * 2, halfHeight * 2, radius * 2));
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, toonShader));
	world->AddGameObject(capsule);
}

void GameManager::AddPxCylinderToWorld(const PxTransform& t, const  PxReal radius, const PxReal halfHeight, float density, float friction, float elasticity)
{
	GameObject* cylinder = new GameObject("Cylinder");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, (2 * halfHeight) - radius), *newMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cylinder->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	cylinder->GetTransform().SetScale(PxVec3(radius * 2, halfHeight * 2, radius * 2));
	cylinder->SetRenderObject(new RenderObject(&cylinder->GetTransform(), cylinderMesh, basicTex, toonShader));
	world->AddGameObject(cylinder);
}

void GameManager::AddBounceSticks(const PxTransform& t, const  PxReal radius, const PxReal halfHeight, float density, float friction, float elasticity) {
	GameObject* capsule = new GameObject("Capsule");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, halfHeight), *newMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	capsule->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	capsule->GetTransform().SetScale(PxVec3(radius * 2, halfHeight * 2, radius * 2));
	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, toonShader));
	world->AddGameObject(capsule);
}

void GameManager::AddPxPickupToWorld(const PxTransform& t, const PxReal radius)
{
	GameObject* p = new GameObject("Pickup");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t);;
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *pXPhysics->GetGMaterial());
	p->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	p->GetTransform().SetScale(PxVec3(radius, radius, radius));
	p->SetRenderObject(new RenderObject(&p->GetTransform(), bonusMesh, basicTex, toonShader));
	p->GetRenderObject()->SetColour(Debug::YELLOW);
	world->AddGameObject(p);
}

PlayerObject* GameManager::AddPxPlayerToWorld(const PxTransform& t, const PxReal scale)
{
	PlayerObject* p = new PlayerObject();

	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
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
	return p;
}

void GameManager::AddPxEnemyToWorld(const PxTransform& t, const PxReal scale)
{
	GameObject* e = new GameObject("Enemy");

	float meshSize = MESH_SIZE * scale;
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
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

void GameManager::AddPxSeeSawToWorld(const PxTransform& t, const PxVec3 halfSizes, float density, float friction, float elasticity)
{
	GameObject* cube = new GameObject("SeeSaw");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
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

void GameManager::AddPxRevolvingDoorToWorld(const PxTransform& t, const PxVec3 halfSizes, float density, float friction, float elasticity)
{
	GameObject* cube = new GameObject("RevolvingDoor");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
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

void GameManager::AddPxRotatingCubeToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 rotation, float friction, float elasticity, string name)
{
	GameObject* cube = new GameObject(name);

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
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

void GameManager::AddPxRotatingCylinderToWorld(const PxTransform& t, const PxReal radius, const PxReal halfHeight, const PxVec3 rotation, float friction, float elasticity)
{
	GameObject* cylinder = new GameObject("RotatingCylinder");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxCapsuleGeometry(radius, (2 * halfHeight) - radius), *newMat)->setLocalPose(PxTransform(PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	body->setAngularVelocity(rotation);
	body->setAngularDamping(0.f);
	body->setMass(0.f);
	body->setMassSpaceInertiaTensor(PxVec3(0.f));
	cylinder->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	cylinder->GetTransform().SetScale(PxVec3(radius * 2, halfHeight * 2, radius * 2));
	cylinder->SetRenderObject(new RenderObject(&cylinder->GetTransform(), cylinderMesh, basicTex, toonShader));
	world->AddGameObject(cylinder);
}


void GameManager::AddPxFloorToWorld(const PxTransform& t, const PxVec3 halfSizes, float friction, float elasticity, TextureState state) {
	GameObject* floor = new GameObject("Floor");

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	floor->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);

	floor->GetTransform().SetScale(halfSizes * 2);
	switch (state) {
		case TextureState::FLOOR:
			floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, toonShader));
			break;
		case TextureState::ICE:
			floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, iceTex, toonShader));
			break;
		case TextureState::INVISIBLE: 
			break;
		
	}
	/*if (!friction)
		floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, iceTex, toonShader));
	else
		floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, floorTex, toonShader));*/
	world->AddGameObject(floor);
}

Cannonball* GameManager::AddPxCannonBallToWorld(const PxTransform& t, const  PxReal radius, const PxVec3* force, float density, float friction, float elasticity)
{
	Cannonball* cannonBall = new Cannonball();
	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t);
	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxSphereGeometry(radius), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cannonBall->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);
	cannonBall->GetTransform().SetScale(PxVec3(radius, radius, radius));
	cannonBall->SetRenderObject(new RenderObject(&cannonBall->GetTransform(), sphereMesh, basicTex, toonShader));
	world->AddGameObject(cannonBall);
	//body->addForce(*force, PxForceMode::eIMPULSE);
	//cannonObj->addShot(cannonBall);

	return cannonBall;
}

void GameManager::AddPxCannonToWorld(const PxTransform& t, const PxVec3 trajectory, const int shotTime, const int shotSize, const PxVec3 translate)
{
	Cannon* cannon = new Cannon(trajectory, shotTime, shotSize, translate);

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(30, 15, 15), *pXPhysics->GetGMaterial());
	cannon->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	cannon->GetTransform().SetScale(PxVec3(30, 15, 15) * 2);
	cannon->SetRenderObject(new RenderObject(&cannon->GetTransform(), cubeMesh, obstacleTex, toonShader));
	world->AddGameObject(cannon);
}

void GameManager::AddPxKillPlaneToWorld(const PxTransform& t, const PxVec3 halfSizes, const PxVec3 respawnCentre, Vector3 respawnSizeRange, bool hide)
{
	KillPlane* cube = new KillPlane(respawnCentre, respawnSizeRange);

	PxRigidStatic* body = pXPhysics->GetGPhysics()->createRigidStatic(t);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *pXPhysics->GetGMaterial());
	cube->SetPhysicsObject(new PhysXObject(body, pXPhysics->GetGMaterial()));
	pXPhysics->GetGScene()->addActor(*body);

	cube->GetTransform().SetScale(halfSizes * 2);
	if (!hide)
	{
		cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, lavaTex, basicShader));
	}
	world->AddGameObject(cube);
}

void GameManager::AddPxFallingTileToWorld(const PxTransform& t, const PxVec3 halfSizes, float density, float friction, float elasticity )
{

	FallingTile* cube = new FallingTile("Floor");

	PxRigidDynamic* body = pXPhysics->GetGPhysics()->createRigidDynamic(t.transform(PxTransform(t.p)));


	PxMaterial* newMat = pXPhysics->GetGPhysics()->createMaterial(friction, friction, elasticity);
	PxRigidActorExt::createExclusiveShape(*body, PxBoxGeometry(halfSizes.x, halfSizes.y, halfSizes.z), *newMat);
	PxRigidBodyExt::updateMassAndInertia(*body, density);
	cube->SetPhysicsObject(new PhysXObject(body, newMat));
	pXPhysics->GetGScene()->addActor(*body);
	body->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, true);
	cube->GetTransform().SetScale(halfSizes * 2);
	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, toonShader));



	world->AddGameObject(cube);
}