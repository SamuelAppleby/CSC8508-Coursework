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
	WorldCreator::Create(pXPhysics, world);
	forceMagnitude = 10.0f;
	useBroadphase = true;
	inSelectionMode = false;
	Debug::SetRenderer(renderer);
	player = nullptr;
	lockedOrientation = true;
	currentLevel = 1;
	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame() {
	delete renderer;
	delete world;
}

void TutorialGame::Update(float dt) {
	pXPhysics->StepPhysics(dt);
	UpdateLevel(dt);
	Debug::FlushRenderables(dt);
	world->UpdateWorld(dt);
	renderer->Update(dt);
	renderer->Render();
}

/* Logic for updating level 1 or level 2 */
void TutorialGame::UpdateLevel(float dt) {
	/* Enter debug mode? */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}

	/* Debug mode selection */
	string message = inSelectionMode ? "Change to play mode(Q)" : "Change to debug mode(Q)";
	renderer->DrawString(message, Vector2(68, 10), Debug::WHITE, textSize);
	if (inSelectionMode) {
		UpdateKeys();
		SelectObject();
		DrawDebugInfo();
		world->ShowFacing();
	}
	
	/* Camera state displayed to user */
	switch (camState) {
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
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1)) {
		switch (camState) {
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
	if (lockedObject) {
		world->GetMainCamera()->UpdateCameraWithObject(dt, lockedObject);
		if (lockedOrientation)
			lockedObject->GetTransform().SetOrientation(PxQuat(world->GetMainCamera()->GetYaw(), { 0, 1, 0 }));
	}
	else if (!inSelectionMode || camState == CameraState::GLOBAL1 || camState == CameraState::GLOBAL2)
		world->GetMainCamera()->UpdateCamera(dt);

	renderer->DrawString("Exit to Menu (ESC)", Vector2(75, 5), Debug::WHITE, textSize);
	if (lockedObject)
		LockedObjectMovement(dt);
	else
		DebugObjectMovement();
}

/* Draws debug information to screen, and will display selected object properties */
void TutorialGame::DrawDebugInfo() {
	string message = world->GetShuffleObjects() ? "Shuffle Objects(F1):On" : "Shuffle Objects(F1):Off";
	renderer->DrawString(message, Vector2(0, 10), Debug::WHITE, textSize);

	renderer->DrawString(message, Vector2(0, 15), Debug::WHITE, textSize);

	renderer->DrawString("Click Force(Scroll Wheel):" + std::to_string((int)forceMagnitude), Vector2(0, 25), Debug::WHITE, textSize);

	if (lockedObject) {
		renderer->DrawString("Unlock object(L)", Vector2(0, 35), Debug::WHITE, textSize);
		message = lockedOrientation ? "Lock object orientation(K): On" : "Lock object orientation(K): Off";
		renderer->DrawString(message, Vector2(0, 40), Debug::WHITE, textSize);
	}
	else
		renderer->DrawString("Lock selected object(L)", Vector2(0, 35), Debug::WHITE, textSize);
	renderer->DrawString("Total Objects:" + std::to_string(world->gameObjects.size()), Vector2(75, 85), Debug::WHITE, textSize);

	/* If selected an object display all its physical properties */
	if (selectionObject) {		
		renderer->DrawString("Selected Object:" + selectionObject->GetName(), Vector2(0, 60), Debug::WHITE, textSize);
		renderer->DrawString("Position:" + Vector3(selectionObject->GetTransform().GetPosition()).ToString(), Vector2(0, 65), Debug::WHITE, textSize);
		renderer->DrawString("Orientation:" + Quaternion(selectionObject->GetTransform().GetOrientation()).ToEuler().ToString(), Vector2(0, 70), Debug::WHITE, textSize);

		if (selectionObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>()) {
			PxRigidDynamic* body = (PxRigidDynamic*)selectionObject->GetPhysicsObject()->GetPXActor();
			renderer->DrawString("Linear Velocity:" + Vector3(body->getLinearVelocity()).ToString(), Vector2(0, 75), Debug::WHITE, textSize);
			renderer->DrawString("Angular Veclocity:" + Vector3(body->getAngularVelocity()).ToString(), Vector2(0, 80), Debug::WHITE, textSize);
			renderer->DrawString("Mass:" + std::to_string(body->getMass()), Vector2(0, 85), Debug::WHITE, textSize);
		}	
		else {
			renderer->DrawString("Linear Velocity:" + Vector3(0, 0, 0).ToString(), Vector2(0, 75), Debug::WHITE, textSize);
			renderer->DrawString("Angular Veclocity:" + Vector3(0, 0, 0).ToString(), Vector2(0, 80), Debug::WHITE, textSize);
			renderer->DrawString("Mass: N/A", Vector2(0, 85), Debug::WHITE, textSize);
		}
		renderer->DrawString("Friction:" + std::to_string(selectionObject->GetPhysicsObject()->GetMaterial()->getDynamicFriction()), Vector2(0, 90), Debug::WHITE, textSize);
		renderer->DrawString("Elasticity:" + std::to_string(selectionObject->GetPhysicsObject()->GetMaterial()->getRestitution()), Vector2(0, 95), Debug::WHITE, textSize);
	}
}

/* In debug mode we can change some of the backend physics engine with some key presses */
void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
		world->ShuffleObjects(!world->GetShuffleObjects());
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 10.0f;
}

/* Initialise camera to default location */
void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.5f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 50, 80));
	world->GetMainCamera()->SetYaw(0);
	world->GetMainCamera()->SetPitch(0);
	lockedObject = nullptr;
}

/* Initialise all the elements contained within the world */
void TutorialGame::InitWorld() {
	world->ClearAndErase();
	//pXPhysics->CleanupPhysics();
	InitFloors(currentLevel);
	InitGameExamples(currentLevel);
	InitGameObstacles(currentLevel);
}

/* Place all the levels solid floors */
void TutorialGame::InitFloors(int level) {
	switch (level) {
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
void TutorialGame::InitGameExamples(int level) {
	switch (level) {
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
void TutorialGame::InitGameObstacles(int level) {
	switch (level) {
	case 1:
		WorldCreator::AddPxSphereToWorld(PxTransform(PxVec3(-20, 20, -20)), 2);
		WorldCreator::AddPxCubeToWorld(PxTransform(PxVec3(0, 20, -20)), PxVec3(2, 2, 2));
		WorldCreator::AddPxCapsuleToWorld(PxTransform(PxVec3(20, 20, -20)), 2, 2);
		break;
	}
}

/* If in debug mode we can select an object with the cursor, displaying its properties and allowing us to take control */
bool TutorialGame::SelectObject() {
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT) && !lockedObject) {
		PxVec3 pos = PhyxConversions::GetVector3(world->GetMainCamera()->GetPosition());
		PxVec3 dir = PhyxConversions::GetVector3(CollisionDetection::GetMouseDirection(*world->GetMainCamera()));
		float distance = 1000.0f;
		PxRaycastBuffer hit;

		if (pXPhysics->GetGScene()->raycast(pos, dir, distance, hit)) {
			if (selectionObject) {
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject->SetSelected(false);
			}
			PxRigidActor* actor = hit.block.actor;
			selectionObject = world->FindObjectFromPhysicsBody(actor);
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
			selectionObject->SetSelected(true);
			return true;
		}
		return false;
	}

	/* We can lock the object and move it around */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				camState = CameraState::FREE;
				lockedObject = nullptr;
			}
			else {
				camState = CameraState::THIRDPERSON;
				lockedObject = selectionObject;
			}
			world->GetMainCamera()->SetState(camState);
		}
	}
	return false;
}

/* If we've selected an object, we can manipulate it with some key presses */
void TutorialGame::DebugObjectMovement() {
	if (inSelectionMode && selectionObject) {
		/* Using the arrow keys and numpad we can twist the object with torque*/
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));

		if (selectionObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>()) {
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
void TutorialGame::LockedObjectMovement(float dt) {
	if (inSelectionMode && selectionObject)
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
	Matrix4 view = world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	Vector3 charForward = Quaternion(lockedObject->GetTransform().GetOrientation()) * Vector3(0, 0, 1);
	float force = 5000.0f * dt;

	if (lockedObject->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>()) {
		PxRigidDynamic* body = (PxRigidDynamic*)selectionObject->GetPhysicsObject()->GetPXActor();

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
			body->addForce(PhyxConversions::GetVector3(fwdAxis) * force, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
			body->addForce(PhyxConversions::GetVector3(-rightAxis) * force, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
			body->addForce(PhyxConversions::GetVector3(-fwdAxis) * force, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
			body->addForce(PhyxConversions::GetVector3(rightAxis) * force, PxForceMode::eIMPULSE);

		body->setLinearVelocity(PhyxConversions::GetVector3(Maths::Clamp(Vector3(body->getLinearVelocity()), Vector3(-15, -50, -15), Vector3(15, 50, 15))));

		/* We can lock the objects orientation with K or swap between camera positons with 1 */
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K))
			lockedOrientation = !lockedOrientation;
	}
	
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1)) {
		switch (camState) {
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
