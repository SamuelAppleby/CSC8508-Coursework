/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game Implementation		 */
#include "TutorialGame.h"
using namespace physx;
using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame() {
	world = new GameWorld();
	renderer = new GameTechRenderer(*world);
	pXPhysics = new PxPhysicsSystem();
	forceMagnitude = 10.0f;
	useBroadphase = true;
	inSelectionMode = false;
	Debug::SetRenderer(renderer);
	player = nullptr;
	lockedOrientation = true;
	currentLevel = 1;
	avgFps = 1.0f;
	framesPerSecond = 0;
	fpsTimer = 1.0f;
	InitCamera();
	WorldCreator::Create(pXPhysics, world); // initialize all textures / mesh / shaders 
}

TutorialGame::~TutorialGame() {
	delete world;
	delete renderer;
	delete pXPhysics;
}

void TutorialGame::ResetWorld() {
	world->ClearAndErase();
	clearCannons();
	//pXPhysics->ResetPhysics();
}

void TutorialGame::Update(float dt) {
	pXPhysics->StepPhysics(dt);
	UpdateLevel(dt);
	world->UpdateWorld(dt);
	renderer->Update(dt);
	renderer->Render();
	Debug::FlushRenderables(dt);
}

/* Logic for updating level 1 or level 2 */
void TutorialGame::UpdateLevel(float dt)
{
	//level2 stuff
	updateCannons(dt);
	updateCannonBalls();

	/* Enter debug mode? */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode)
		{
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else
		{
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	/* Debug mode selection */
	string message = inSelectionMode ? "Change to play mode(Q)" : "Change to debug mode(Q)";
	renderer->DrawString(message, Vector2(68, 10), Debug::WHITE, textSize);
	if (inSelectionMode)
	{
		UpdateKeys();
		SelectObject();
		DrawDebugInfo();
		world->ShowFacing();
	}
	/* Rolling FPS calculations */
	fpsTimer -= dt;
	++framesPerSecond;
	if (fpsTimer < 0.0f) {
		float alpha = 0.1f;
		avgFps = alpha * avgFps + (1.0 - alpha) * framesPerSecond;
		framesPerSecond = 0;
		fpsTimer = 1.0f;
	}
	renderer->DrawString("FPS:" + std::to_string(avgFps), Vector2(0, 5), Debug::WHITE, 15.0f);
	
	/* Camera state displayed to user */
	switch (camState)
	{
	case CameraState::FREE:
		renderer->DrawString("Change to Global Camera[1]", Vector2(62, 20), Debug::WHITE, textSize);
		break;
	case CameraState::GLOBAL1:
		renderer->DrawString("Change to Free Camera[1]", Vector2(65, 20), Debug::WHITE, textSize);
		break;
	case CameraState::GLOBAL2:
		renderer->DrawString("Change to Free Camera[1]", Vector2(65, 20), Debug::WHITE, textSize);
		break;
	case CameraState::THIRDPERSON:
		renderer->DrawString("Change to Topdown Camera[1]", Vector2(61, 20), Debug::WHITE, textSize);
		break;
	case CameraState::TOPDOWN:
		renderer->DrawString("Change to Thirdperson Camera[1]", Vector2(56, 20), Debug::WHITE, textSize);
		break;
	}

	/* Change Camera */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1))
	{
		switch (camState)
		{
		case CameraState::FREE:
			camState = currentLevel == 1 ? CameraState::GLOBAL1 : CameraState::GLOBAL2;
			break;
		case CameraState::GLOBAL1:
			InitCamera();
			camState = CameraState::FREE;
			break;
		case CameraState::GLOBAL2:
			InitCamera();
			camState = CameraState::FREE;
			break;
		}
		world->GetMainCamera()->SetState(camState);
	}

	/* Change how we move the camera dependng if we have a locked object */
	if (lockedObject != nullptr) {
		if (lockedObject)
		{
			world->GetMainCamera()->UpdateCameraWithObject(dt, lockedObject);
			if (lockedOrientation)
			{
				if (lockedObject->GetPhysicsObject() != nullptr) {
					PxRigidDynamic* actor = (PxRigidDynamic*)lockedObject->GetPhysicsObject()->GetPXActor();
					actor->setAngularVelocity(PxVec3(0));
					float yaw = world->GetMainCamera()->GetYaw();
					yaw = Maths::DegreesToRadians(yaw);
					actor->setGlobalPose(PxTransform(actor->getGlobalPose().p, PxQuat(yaw, { 0, 1, 0 })));
					Window::GetWindow()->ShowOSPointer(false);
					Window::GetWindow()->LockMouseToWindow(true);
					PxTransform pose = actor->getGlobalPose();
					Vector3 camPos = Quaternion(pose.q.x, pose.q.y, pose.q.z, pose.q.w) * Vector3(0, 5, 30) + pose.p;
					world->GetMainCamera()->SetPosition(camPos);
				}

			}
		}
	}

	else if (!inSelectionMode || camState == CameraState::GLOBAL1 || camState == CameraState::GLOBAL2)
		world->GetMainCamera()->UpdateCamera(dt);

	renderer->DrawString("Exit to Menu (ESC)", Vector2(75, 5), Debug::WHITE, textSize);
	if (lockedObject)
		LockedObjectMovement(dt);
	else
		if (selectionObject != nullptr) {
			if (selectionObject->GetRenderObject() != nullptr) {
				DebugObjectMovement();
			}
		}

}

/* Draws debug information to screen, and will display selected object properties */
void TutorialGame::DrawDebugInfo()
{
	string message = world->GetShuffleObjects() ? "Shuffle Objects(F1):On" : "Shuffle Objects(F1):Off";
	renderer->DrawString(message, Vector2(0, 10), Debug::WHITE, textSize);

	renderer->DrawString(message, Vector2(0, 15), Debug::WHITE, textSize);

	renderer->DrawString("Click Force(Scroll Wheel):" + std::to_string((int)forceMagnitude), Vector2(0, 25), Debug::WHITE, textSize);

	if (lockedObject)
	{
		renderer->DrawString("Unlock object(L)", Vector2(0, 35), Debug::WHITE, textSize);
		message = lockedOrientation ? "Lock object orientation(K): On" : "Lock object orientation(K): Off";
		renderer->DrawString(message, Vector2(0, 40), Debug::WHITE, textSize);
	}
	else
		renderer->DrawString("Lock selected object(L)", Vector2(0, 35), Debug::WHITE, textSize);
	renderer->DrawString("Static Physics Objects:" + std::to_string(pXPhysics->GetGScene()->getNbActors(PxActorTypeFlag::eRIGID_STATIC)), Vector2(65, 70), Debug::WHITE, textSize);
	renderer->DrawString("Dynamic Physics Objects:" + std::to_string(pXPhysics->GetGScene()->getNbActors(PxActorTypeFlag::eRIGID_DYNAMIC)), Vector2(63, 75), Debug::WHITE, textSize);
	renderer->DrawString("Total Game Objects:" + std::to_string(world->gameObjects.size()), Vector2(67, 80), Debug::WHITE, textSize);
	renderer->DrawString("Current Collisions:" + std::to_string(world->GetTotalCollisions()), Vector2(70, 85), Debug::WHITE, 15.0f);

	/* If selected an object display all its physical properties */
	if (selectionObject)
	{
		renderer->DrawString("Selected Object:" + selectionObject->GetName(), Vector2(0, 60), Debug::WHITE, textSize);
		renderer->DrawString("Position:" + Vector3(selectionObject->GetTransform().GetPosition()).ToString(), Vector2(0, 65), Debug::WHITE, textSize);
		renderer->DrawString("Orientation:" + Quaternion(selectionObject->GetTransform().GetOrientation()).ToEuler().ToString(), Vector2(0, 70), Debug::WHITE, textSize);

		if (selectionObject->GetPhysicsObject() != nullptr && selectionObject->GetPhysicsObject()->GetPXActor() != nullptr) {
			if (selectionObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
			{
				PxRigidDynamic* body = (PxRigidDynamic*)selectionObject->GetPhysicsObject()->GetPXActor();
				renderer->DrawString("Linear Velocity:" + Vector3(body->getLinearVelocity()).ToString(), Vector2(0, 75), Debug::WHITE, textSize);
				renderer->DrawString("Angular Veclocity:" + Vector3(body->getAngularVelocity()).ToString(), Vector2(0, 80), Debug::WHITE, textSize);
				renderer->DrawString("Mass:" + std::to_string(body->getMass()), Vector2(0, 85), Debug::WHITE, textSize);
			}
			else
			{
				renderer->DrawString("Linear Velocity:" + Vector3(0, 0, 0).ToString(), Vector2(0, 75), Debug::WHITE, textSize);
				renderer->DrawString("Angular Veclocity:" + Vector3(0, 0, 0).ToString(), Vector2(0, 80), Debug::WHITE, textSize);
				renderer->DrawString("Mass: N/A", Vector2(0, 85), Debug::WHITE, textSize);
			}
			renderer->DrawString("Friction:" + std::to_string(selectionObject->GetPhysicsObject()->GetMaterial()->getDynamicFriction()), Vector2(0, 90), Debug::WHITE, textSize);
			renderer->DrawString("Elasticity:" + std::to_string(selectionObject->GetPhysicsObject()->GetMaterial()->getRestitution()), Vector2(0, 95), Debug::WHITE, textSize);
		}

	}
}

/* In debug mode we can change some of the backend physics engine with some key presses */
void TutorialGame::UpdateKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
		world->ShuffleObjects(!world->GetShuffleObjects());
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 10.0f;
}

/* Initialise camera to default location */
void TutorialGame::InitCamera()
{
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 50, 80));
	world->GetMainCamera()->SetYaw(0);
	world->GetMainCamera()->SetPitch(0);
	lockedObject = nullptr;
}

/* Initialise all the elements contained within the world */
void TutorialGame::InitWorld()
{
	
	//pXPhysics->CleanupPhysics();
	InitFloors(currentLevel);
	InitGameExamples(currentLevel);
	InitGameObstacles(currentLevel);
}

//void Tu

/* Place all the levels solid floors */
void TutorialGame::InitFloors(int level)
{
	switch (level)
	{
	case 0:
		break;
	case 1:
		WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -20, 0)), PxVec3(100, 1, 100));
		break;
	case 2:
		break;
	}
}

/* Initialises all game objects, enemies etc */
void TutorialGame::InitGameExamples(int level)
{
	switch (level)
	{
	case 0:
		break;
	case 1:
		WorldCreator::AddPxPickupToWorld(PxTransform(PxVec3(-20, 20, 0)), 1);
		WorldCreator::AddPxPlayerToWorld(PxTransform(PxVec3(0, 20, 0)), 1);
		WorldCreator::AddPxEnemyToWorld(PxTransform(PxVec3(20, 20, 0)), 1);
		break;
	case 2:
		break;
	}
}

/* This method will initialise any other moveable obstacles we want */
void TutorialGame::InitGameObstacles(int level)
{
	switch (level)
	{
	case 1:
		WorldCreator::AddPxSphereToWorld(PxTransform(PxVec3(-20, 20, -20)), 2);
		WorldCreator::AddPxCubeToWorld(PxTransform(PxVec3(0, 20, -20)), PxVec3(2, 2, 2));
		WorldCreator::AddPxCapsuleToWorld(PxTransform(PxVec3(20, 20, -20)), 2, 2);
		break;
	}
}

/* If in debug mode we can select an object with the cursor, displaying its properties and allowing us to take control */
bool TutorialGame::SelectObject()
{
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT) && !lockedObject)
	{
		PxVec3 pos = PhyxConversions::GetVector3(world->GetMainCamera()->GetPosition());
		PxVec3 dir = PhyxConversions::GetVector3(CollisionDetection::GetMouseDirection(*world->GetMainCamera()));
		float distance = 1000.0f;
		PxRaycastBuffer hit;

		if (pXPhysics->GetGScene()->raycast(pos, dir, distance, hit))
		{
			if (selectionObject != nullptr) {
				if (selectionObject->GetRenderObject() != nullptr) {
					selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
					selectionObject->SetSelected(false);
				}
			}


			PxRigidActor* actor = hit.block.actor;
			selectionObject = world->FindObjectFromPhysicsBody(actor);


			if (selectionObject->GetRenderObject()) {
					selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

				
			}
			selectionObject->SetSelected(true);
			return true;
		}
		return false;
	}

	/* We can lock the object and move it around */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L))
	{
		if (selectionObject)
		{
			if (lockedObject == selectionObject)
			{
				camState = CameraState::FREE;
				lockedObject = nullptr;
			}
			else
			{
				camState = CameraState::THIRDPERSON;
				lockedObject = selectionObject;
			}
			world->GetMainCamera()->SetState(camState);
		}
	}
	return false;
}

/* If we've selected an object, we can manipulate it with some key presses */
void TutorialGame::DebugObjectMovement()
{
	if (inSelectionMode && selectionObject)
	{
		/* Using the arrow keys and numpad we can twist the object with torque*/
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

		if (selectionObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
		{
			PxRigidDynamic* body = (PxRigidDynamic*)selectionObject->GetPhysicsObject()->GetPXActor();

			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT))
				body->addTorque(PxVec3(-10, 0, 0), PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT))
				body->addTorque(PxVec3(10, 0, 0), PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP))
				body->addTorque(PxVec3(0, 0, -10), PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN))
				body->addTorque(PxVec3(0, 0, 10), PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM2))
				body->addTorque(PxVec3(0, -10, 0), PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8))
				body->addTorque(PxVec3(0, 10, 0), PxForceMode::eIMPULSE);
		}
	}
}

/* If we have control of an object we can move it around and perform certain actions */
void TutorialGame::LockedObjectMovement(float dt)
{
	if (inSelectionMode && selectionObject)
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	Vector3 charForward = Quaternion(lockedObject->GetTransform().GetOrientation()) * Vector3(0, 0, 1);
	float force = 1200.0f;

	if (lockedObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
	{
		PxRigidDynamic* body = (PxRigidDynamic*)selectionObject->GetPhysicsObject()->GetPXActor();
		body->setLinearDamping(0.4f);

		if (lockedObject->IsGrounded())
		{
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
				body->addForce(PhyxConversions::GetVector3(fwdAxis) * force, PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
				body->addForce(PhyxConversions::GetVector3(-rightAxis) * force, PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
				body->addForce(PhyxConversions::GetVector3(-fwdAxis) * force, PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
				body->addForce(PhyxConversions::GetVector3(rightAxis) * force, PxForceMode::eIMPULSE);
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && lockedObject->IsGrounded()) {
				body->addForce(PhyxConversions::GetVector3(Vector3(0, 1, 0)) * 20000, PxForceMode::eIMPULSE);
				//lockedObject->SetGrounded(false);
			}
		}

		/* We can lock the objects orientation with K or swap between camera positons with 1 */
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K))
			lockedOrientation = !lockedOrientation;
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1))
	{
		switch (camState)
		{
		case CameraState::THIRDPERSON:
			camState = CameraState::TOPDOWN;
			break;
		case CameraState::TOPDOWN:
			camState = CameraState::THIRDPERSON;
			break;
		}
		world->GetMainCamera()->SetState(camState);
	}
}

void TutorialGame::initLevel2() {

	world->GetMainCamera()->SetFarPlane(10000.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, -86, -1050));



	//player added to check this is all a reasonable scale
	WorldCreator::AddPxPlayerToWorld(PxTransform(PxVec3(0, 57, -1411)), 1);


	//fyi, the buffer zones go between obstacles. This is to give the player time to think so it's not just all one muscle memory dash
	//(that way, it also allows time for other players to catch up and makes each individual obstacle more chaotic, so it's a double win)


	//first we'll do the floors
	//starting zone
	//floor
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 0, 0)), PxVec3(200, 1, 100));

	//back wall
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 4.5, 50)), PxVec3(200, 10, 1));
	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, 4.5, 0)), PxVec3(1, 10, 100));
	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, 4.5, 0)), PxVec3(1, 10, 100));

	//OBSTACLE 0.5 - THE RAMP DOWN
	//after a short platform for players to build up some momentum, the players are sent down a ramp to build up speed
	//it should be slippery as well (possibly do some stuff with coefficients?) so that players don't have as much control

	//first ramp down (maybe some stuff to make it slippery after, it's designed to be like the total wipe out slide
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -15.122, -199.85), PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(200, 1, 300), 0, 0);
	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, -11, -199.8), PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(1, 10, 305));
	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, -11, -199.8), PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(1, 10, 305));


	//buffer zone 1 (where contestants respawn on failing the first obstacle, this needs to be sorted on the individual kill plane)

	Vector3 respawnSize = Vector3(180, 0, 80);
	PxVec3 zone1Position = PxVec3(0, -87, -384);
	//bottom kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -325)), PxVec3(500, 1, 850), zone1Position, respawnSize);
	//back kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 100, 100)), PxVec3(500, 500, 1), zone1Position, respawnSize);
	//left side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -325)), PxVec3(1, 500, 850), zone1Position, respawnSize);
	//right side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -325)), PxVec3(1, 500, 850), zone1Position, respawnSize);

	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -384)), PxVec3(200, 1, 100));
	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, -84, -384)), PxVec3(1, 10, 100));
	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, -84, -384)), PxVec3(1, 10, 100));

	//OBSTACLE 1 - THE STEPPING STONE TRAMPOLINES
	//supposed to be bouncy floors/stepping stones type things, think like a combo of the trampolines from the end of splash mountain in total wipeout,
	//and the stepping stones from takeshi's castle
	//may need to add more if the jump isn't far enough
	//honestly not sure on some of these values, there may not be enough stepping stones, so we should decide if more are needed after a little testing
	//front row
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-60, -88, -470)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -480)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(50, -88, -475)), PxVec3(30, 1, 30));
	//next row
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-40, -88, -550)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(10, -88, -540)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(60, -88, -520)), PxVec3(30, 1, 30));
	//last row
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-65, -88, -610)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-20, -88, -620)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(20, -88, -640)), PxVec3(30, 1, 30));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, -88, -600)), PxVec3(30, 1, 30));


	//buffer zone 2 (where contestants respawn on failing the second obstacle, this needs to be sorted on the individual kill plane)

	PxVec3 zone2Position = PxVec3(0, -87, -750);

	//bottom kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -900)), PxVec3(500, 1, 300), zone2Position, respawnSize);
	//left side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -900)), PxVec3(1, 500, 300), zone2Position, respawnSize);
	//right side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -900)), PxVec3(1, 500, 300), zone2Position, respawnSize);

	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -750)), PxVec3(200, 1, 100));
	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, -58, -750)), PxVec3(1, 60, 100));
	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, -58, -750)), PxVec3(1, 60, 100));

	//gate for buffer zone 2 (prevents players jumping ahead on the spinning platforms)
	//top of gate, may need to give this some height if there are problems getting onto platforms from jumping
	//have made the front walls taller to accommodate this, so only the top piece (below) needs to be adjusted
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -48, -800)), PxVec3(200, 40, 1));

	//gate left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-90, -68, -800)), PxVec3(20, 40, 1));

	//gate left mid
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-35, -68, -800)), PxVec3(50, 40, 1));

	//gate right mid
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(35, -68, -800)), PxVec3(50, 40, 1));

	//gate right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(90, -68, -800)), PxVec3(20, 40, 1));

	//OBSTACLE 3 - THE SPINNING COLUMNS (WITH CANNONS EITHER SIDE)
	//I have no idea how to make these spin
	//I've added three so there's ample gaps between them to fall down, but allows more than one queue for efficiency
	//also that's what I put in the design and it looked nice
	//may need to make this a different class so it can spin, but I'm making it a floor for the prototype so we can see the level layout more easily
	//if they can't be made to spin, it might be an idea to switch these out for capsules. Should have a similar balance difficulty

	//HAVE COMMENTED OUT THE ORIGINAL BEAMS, WILL LEAVE IN IN CASE WE DECIDE TO GO FOR STATIC ONES
	//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 200));
	WorldCreator::AddPxRotatingCubeToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 198), new const PxVec3(0, 0, 10));

	//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -98, -900)), PxVec3(20, 20, 200));
	WorldCreator::AddPxRotatingCubeToWorld(PxTransform(PxVec3(0, -98, -900)), PxVec3(20, 20, 198), new const PxVec3(0, 0, 10));

	//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, -98, -900)), PxVec3(20, 20, 200));
	WorldCreator::AddPxRotatingCubeToWorld(PxTransform(PxVec3(70, -98, -900)), PxVec3(20, 20, 198), new const PxVec3(0, 0, 10));

	//cannons
	Cannon* cannon2 = new Cannon(new PxVec3(-150, -80, -850), new PxVec3(7000000, 8500, 0), 10, 10);
	Cannon* cannon3 = new Cannon(new PxVec3(-150, -80, -900), new PxVec3(7000000, 8500, 0), 10, 10);
	Cannon* cannon4 = new Cannon(new PxVec3(-150, -80, -950), new PxVec3(7000000, 8500, 0), 10, 10);

	Cannon* cannon5 = new Cannon(new PxVec3(150, -80, -825), new PxVec3(-7000000, 8500, 0), 10, 10);
	Cannon* cannon6 = new Cannon(new PxVec3(150, -80, -875), new PxVec3(-7000000, 8500, 0), 10, 10);
	Cannon* cannon7 = new Cannon(new PxVec3(150, -80, -925), new PxVec3(-7000000, 8500, 0), 10, 10);
	Cannon* cannon8 = new Cannon(new PxVec3(150, -80, -975), new PxVec3(-7000000, 8500, 0), 10,10);

	WorldCreator::AddPxCannonToWorld(cannon2);
	WorldCreator::AddPxCannonToWorld(cannon3);
	WorldCreator::AddPxCannonToWorld(cannon4);
	WorldCreator::AddPxCannonToWorld(cannon5);
	WorldCreator::AddPxCannonToWorld(cannon6);
	WorldCreator::AddPxCannonToWorld(cannon7);
	WorldCreator::AddPxCannonToWorld(cannon8);

	cannons.push_back(cannon2);
	cannons.push_back(cannon3);
	cannons.push_back(cannon4);
	cannons.push_back(cannon5);
	cannons.push_back(cannon6);
	cannons.push_back(cannon7);
	cannons.push_back(cannon8);

	//buffer zone 3 (where contestants respawn on failing the third obstacle, this needs to be sorted on the individual kill plane)

	PxVec3 zone3Position = PxVec3(0, -87, -1050);
	//bottom kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -1231)), PxVec3(500, 1, 362), zone3Position, respawnSize);
	//left side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -1231)), PxVec3(1, 500, 362), zone3Position, respawnSize);
	//right side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -1231)), PxVec3(1, 500, 362), zone3Position, respawnSize);


	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1050)), PxVec3(200, 1, 100));
	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, -58, -1050)), PxVec3(1, 60, 100));
	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, -58, -1050)), PxVec3(1, 60, 100));

	//this time, the gate is to stop the bowling balls hitting players in the buffer zone. 
	//could also act as a kill plane for the bowling balls
	//top of gate
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -38, -1100)), PxVec3(200, 20, 1));

	//gate left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-90, -68, -1100)), PxVec3(20, 40, 1));

	//gate left mid
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-35, -68, -1100)), PxVec3(50, 40, 1));

	//gate right mid
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(35, -68, -1100)), PxVec3(50, 40, 1));

	//gate right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(90, -68, -1100)), PxVec3(20, 40, 1));

	//OBSTACLE 4 - RAMPED BOWLING ALLEY
	//so basically it's like that one bit of mario kart, but also indiana jones, takeshi's castle, and probably some other stuff
	//media tends to be surprisingly boulder centric
	//I thought it'd be fun if they were bowling balls rolling down a hill, and you were trying not to get hit 
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -331, -1225), PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(200, 1, 300),1);

	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, -325, -1225), PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(1, 12, 310));

	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, -325, -1225), PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(1, 12, 310));

	//pegs as obstacles/hiding places for the bowling balls
	//row 1
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-60, 570, -1180), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 570, -1180), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(60, 570, -1180), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

	//row 2
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-30, 645, -1235), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(30, 645, -1235), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

	//row 3

	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-60, 720, -1290), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 720, -1290), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(60, 720, -1290), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

	Cannon* c1 = new Cannon(new PxVec3(-80, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c2 = new Cannon(new PxVec3(-60, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c3 = new Cannon(new PxVec3(-40, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c4 = new Cannon(new PxVec3(-20, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c5 = new Cannon(new PxVec3(0, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c6 = new Cannon(new PxVec3(20, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c7 = new Cannon(new PxVec3(40, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c8 = new Cannon(new PxVec3(60, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);
	Cannon* c9 = new Cannon(new PxVec3(80, 55, -1351), new PxVec3(1, 1, 70000000), 30, 15, 10000);

	cannons.push_back(c1);
	cannons.push_back(c2);
	cannons.push_back(c3);
	cannons.push_back(c4);
	cannons.push_back(c5);
	cannons.push_back(c6);
	cannons.push_back(c7);
	cannons.push_back(c8);
	cannons.push_back(c9);

	//buffer zone 4 (where contestants respawn on failing the fourth obstacle, this needs to be sorted on the individual kill plane)
	PxVec3 zone4Position = PxVec3(0, 56, -1411);

	//bottom kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -1631)), PxVec3(500, 1, 438), zone4Position, respawnSize);
	//left side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -1631)), PxVec3(1, 500, 438), zone4Position, respawnSize);
	//right side kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -1631)), PxVec3(1, 500, 438), zone4Position, respawnSize);
	//back kill plane
	WorldCreator::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 100, -1850)), PxVec3(500, 500, 1), zone4Position, respawnSize);


	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 55, -1411)), PxVec3(200, 1, 100));
	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, 81, -1411)), PxVec3(1, 52, 100));
	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, 81, -1411)), PxVec3(1, 52, 100));

	//diving boards can be used to give players an advantage in getting further into the blender
	//(if they can stay on, it's not gated to encourage players barging into each other

	//diving board left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-70, 55, -1495)), PxVec3(20, 1, 70), 0.5, 2);

	//diving board centre
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 55, -1495)), PxVec3(20, 1, 70), 0.5, 1.5);

	//diving board right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, 55, -1495)), PxVec3(20, 1, 70), 0.5, 1.5);

	//OBSTACLE 5 - THE BLENDER
	//basically, it's an enclosed space with a spinning arm at the bottom to randomise which player actually wins
	//it should be flush with the entrance to the podium room so that the door is reasonably difficult to access unless there's nobody else there
	//again, not sure how to create the arm, it's a moving object, might need another class for this
	//also, it's over a 100m drop to the blender floor, so pls don't put fall damage in
	//blender blade
	WorldCreator::AddPxRotatingCubeToWorld(PxTransform(PxVec3(0, -73, -1700)), PxVec3(190, 20, 20), new const PxVec3(0, 50, 0), 0.5f, 0.100000000015F, "BlenderBlade");

	//blender floor
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1630.5)), PxVec3(200, 1, 339));

	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-100, 8, -1630.5)), PxVec3(1, 200, 339));

	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(100, 8, -1630.5)), PxVec3(1, 200, 339));

	//back wall
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -16.5, -1461)), PxVec3(200, 143, 1));

	//should leave a gap of 20. Enough to get through, but tricky when there's a big old blender shoving everyone around
	//inspired loosely by those windmills on crazy golf
	//front wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-55, -58, -1800)), PxVec3(90, 60, 1));

	//front wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(55, -58, -1800)), PxVec3(90, 60, 1));

	//front wall top
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 23, -1800)), PxVec3(200, 170, 1));

	//roof
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, 107, -1580.5)), PxVec3(200, 1, 439));


	//VICTORY PODIUM
	//the room you have to get in after the blender
	//press the button to win
	//(button + podium yet to be added)
	//also want a sliding door that locks players out
	//pressing the button should make all the floors drop out
	//Podium floor
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1825)), PxVec3(50, 1, 50));

	//side wall left
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-25, -63, -1825)), PxVec3(1, 50, 50));

	//side wall right
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(25, -63, -1825)), PxVec3(1, 50, 50));

	//end wall 
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -63, -1850)), PxVec3(50, 50, 1));

	//roof
	WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -38, -1825)), PxVec3(50, 1, 50));
}

void TutorialGame::updateCannons(float dt) {
	for (int i = 0; i < cannons.size(); ++i) {
		cannons[i]->Update(dt);
		if (cannons[i]->getTimeSinceShot() > 5) {
			Vector3 position = cannons[i]->GetTransform().GetPosition();
			WorldCreator::AddPxCannonBallToWorld(PxTransform(*cannons[i]->getCannonBallPosition()), cannons[i], cannons[i]->getShotSize(), cannons[i]->shoot(), cannons[i]->getShotDensity());
		}
	}
}

void TutorialGame::updateCannonBalls() {
	for (int i = 0; i < cannons.size(); ++i) {
		for (int j = 0; j < cannons[i]->getShots().size(); ++j) {
			if ((cannons[i]->getShots()[j]->getDestroy() || (cannons[i]->getShots()[j]->GetTimeAlive() > cannons[i]->getMaxAlive()))) {
				pXPhysics->GetGScene()->removeActor(*cannons[i]->getShots()[j]->getBody());
				cannons[i]->getShots()[j]->SetRenderObject(NULL);
				cannons[i]->getShots()[j]->SetPhysicsObject(NULL);
				world->RemoveGameObject(cannons[i]->getShots()[j], false);
				cannons[i]->removeShot(cannons[i]->getShots()[j]);
			}
		}
	}
}

void TutorialGame::clearCannons() {
	for (int i = 0; i < cannons.size(); ++i) {
		for (int j = 0; j < cannons[i]->getShots().size(); ++j) {
				cannons[i]->getShots()[j]->SetRenderObject(NULL);
				cannons[i]->getShots()[j]->SetPhysicsObject(NULL);
				world->RemoveGameObject(cannons[i]->getShots()[j], false);
				cannons[i]->removeShot(cannons[i]->getShots()[j]);
		}
	}

	cannons.clear();
}