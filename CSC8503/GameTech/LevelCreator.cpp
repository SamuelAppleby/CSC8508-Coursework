/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game Implementation		 */
#include "LevelCreator.h"
using namespace physx;
using namespace NCL;
using namespace CSC8503;

LevelCreator::LevelCreator()
{
	player = nullptr;
	GameManager::Create(new PxPhysicsSystem(), new GameWorld(), new AudioManager());
	renderer = new GameTechRenderer(*GameManager::GetWorld());
	Debug::SetRenderer(renderer);
	GameManager::CreateGraphics();
}

LevelCreator::~LevelCreator()
{
	delete renderer;
}

void LevelCreator::ResetWorld()
{
	GameManager::GetWorld()->ClearAndErase();
	//clearCannons();
	//WorldCreator::GetPhysicsSystem()->ResetPhysics();
}

void LevelCreator::Update(float dt)
{
	GameManager::GetPhysicsSystem()->StepPhysics(dt);
	UpdateLevel(dt);
	GameManager::GetWorld()->UpdateWorld(dt);
	renderer->Update(dt);
	renderer->Render();
	Debug::FlushRenderables(dt);
}

/* Logic for updating level 1 or level 2 */
void LevelCreator::UpdateLevel(float dt)
{
	//level2 stuff
	updateCannons(dt);
	updateCannonBalls();

	/* Enter debug mode? */
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q))
	{
		if (GameManager::GetLevelState() != LevelState::DEBUG)
		{
			currentLevel = GameManager::GetLevelState();
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
			GameManager::SetLevelState(LevelState::DEBUG);
		}
		else
		{
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
			GameManager::SetLevelState(currentLevel);
		}
	}

	/* Debug mode selection */
	if (GameManager::GetLevelState() == LevelState::DEBUG)
	{
		UpdateKeys();
		SelectObject();
		GameManager::GetWorld()->ShowFacing();
	}

	/* Change Camera */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1))
	{
		switch (GameManager::GetCameraState())
		{
		case CameraState::FREE:
			if (GameManager::GetLevelState() == LevelState::LEVEL1)
				GameManager::SetCamMode(CameraState::GLOBAL1);
			else
				GameManager::SetCamMode(CameraState::GLOBAL2);
			break;
		case CameraState::GLOBAL1:
			InitCamera();
			GameManager::SetCamMode(CameraState::FREE);
			break;
		case CameraState::GLOBAL2:
			InitCamera();
			GameManager::SetCamMode(CameraState::FREE);
			break;
		}
		GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetCameraState());
	}

	/* Change how we move the camera dependng if we have a locked object */
	if (GameManager::GetLockedObject() != nullptr)
	{
		PxRigidDynamic* actor = (PxRigidDynamic*)GameManager::GetLockedObject()->GetPhysicsObject()->GetPXActor();
		if (GameManager::GetLockedObject()->GetPhysicsObject()->GetPXActor()->is<PxRigidBody>()) actor->setAngularVelocity(PxVec3(0));
		float yaw = GameManager::GetWorld()->GetMainCamera()->GetYaw();
		yaw = Maths::DegreesToRadians(yaw);
		actor->setGlobalPose(PxTransform(actor->getGlobalPose().p, PxQuat(yaw, { 0, 1, 0 })));
		GameManager::GetWorld()->GetMainCamera()->UpdateCameraWithObject(dt, GameManager::GetLockedObject());
	}

	else if (GameManager::GetLevelState() != LevelState::DEBUG || GameManager::GetCameraState() == CameraState::GLOBAL1 || GameManager::GetCameraState() == CameraState::GLOBAL2)
		GameManager::GetWorld()->GetMainCamera()->UpdateCamera(dt);

	if (GameManager::GetLockedObject())
		LockedObjectMovement(dt);
	else if (GameManager::GetSelectionObject()) {
		DebugObjectMovement();
	}
}

/* In debug mode we can change some of the backend physics engine with some key presses */
void LevelCreator::UpdateKeys()
{
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1))
		GameManager::GetWorld()->ShuffleObjects(!GameManager::GetWorld()->GetShuffleObjects());
}

/* Initialise camera to default location */
void LevelCreator::InitCamera()
{
	GameManager::GetWorld()->GetMainCamera()->SetNearPlane(0.5f);
	GameManager::GetWorld()->GetMainCamera()->SetFarPlane(10000.0f);
	GameManager::GetWorld()->GetMainCamera()->SetPosition(Vector3(0, 50, 80));
	GameManager::GetWorld()->GetMainCamera()->SetYaw(0);
	GameManager::GetWorld()->GetMainCamera()->SetPitch(0);
	GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::FREE);
	GameManager::SetCamMode(CameraState::FREE);
}

/* Initialise all the elements contained within the world */
void LevelCreator::InitWorld(LevelState state)
{
	GameManager::SetLevelState(state);
	InitFloors(state);
	InitGameExamples(state);
	InitGameObstacles(state);
	InitCamera();
}

/* Place all the levels solid floors */
void LevelCreator::InitFloors(LevelState state)
{
	switch (state)
	{
	case LevelState::MENU:
		break;
	case LevelState::LEVEL1:
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -20, 0)), PxVec3(100, 1, 100));
		break;
	case LevelState::LEVEL2:
		//fyi, the buffer zones go between obstacles. This is to give the player time to think so it's not just all one muscle memory dash
		//(that way, it also allows time for other players to catch up and makes each individual obstacle more chaotic, so it's a double win)
		//first we'll do the floors
		//starting zone
		//floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 0, 0)), PxVec3(200, 1, 100));
		//back wall
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 4.5, 50)), PxVec3(200, 10, 1));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, 4.5, 0)), PxVec3(1, 10, 100));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, 4.5, 0)), PxVec3(1, 10, 100));

		//OBSTACLE 0.5 - THE RAMP DOWN
		//after a short platform for players to build up some momentum, the players are sent down a ramp to build up speed
		//it should be slippery as well (possibly do some stuff with coefficients?) so that players don't have as much control

		//first ramp down (maybe some stuff to make it slippery after, it's designed to be like the total wipe out slide
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -15.122, -199.85), PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(200, 1, 300), 0, 0);
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -11, -199.8), PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(1, 10, 305));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -11, -199.8), PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(1, 10, 305));

		//buffer zone 1 (where contestants respawn on failing the first obstacle, this needs to be sorted on the individual kill plane)
		Vector3 respawnSize = Vector3(180, 0, 80);
		PxVec3 zone1Position = PxVec3(0, -87, -384);
		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -325)), PxVec3(500, 1, 850), zone1Position, respawnSize);
		//back kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 100, 100)), PxVec3(500, 500, 1), zone1Position, respawnSize);
		//left side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -325)), PxVec3(1, 500, 850), zone1Position, respawnSize);
		//right side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -325)), PxVec3(1, 500, 850), zone1Position, respawnSize);

		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -384)), PxVec3(200, 1, 100));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -84, -384)), PxVec3(1, 10, 100));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -84, -384)), PxVec3(1, 10, 100));

		//OBSTACLE 1 - THE STEPPING STONE TRAMPOLINES
		//supposed to be bouncy floors/stepping stones type things, think like a combo of the trampolines from the end of splash mountain in total wipeout,
		//and the stepping stones from takeshi's castle
		//may need to add more if the jump isn't far enough
		//honestly not sure on some of these values, there may not be enough stepping stones, so we should decide if more are needed after a little testing
		//front row
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-60, -88, -470)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -480)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, -88, -475)), PxVec3(30, 1, 30));
		//next row
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-40, -88, -550)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(10, -88, -540)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(60, -88, -520)), PxVec3(30, 1, 30));
		//last row
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-65, -88, -610)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-20, -88, -620)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(20, -88, -640)), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(70, -88, -600)), PxVec3(30, 1, 30));

		//buffer zone 2 (where contestants respawn on failing the second obstacle, this needs to be sorted on the individual kill plane)
		PxVec3 zone2Position = PxVec3(0, -87, -750);

		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -900)), PxVec3(500, 1, 300), zone2Position, respawnSize);
		//left side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -900)), PxVec3(1, 500, 300), zone2Position, respawnSize);
		//right side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -900)), PxVec3(1, 500, 300), zone2Position, respawnSize);

		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -750)), PxVec3(200, 1, 100));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -58, -750)), PxVec3(1, 60, 100));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -58, -750)), PxVec3(1, 60, 100));

		//gate for buffer zone 2 (prevents players jumping ahead on the spinning platforms)
		//top of gate, may need to give this some height if there are problems getting onto platforms from jumping
		//have made the front walls taller to accommodate this, so only the top piece (below) needs to be adjusted
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -48, -800)), PxVec3(200, 40, 1));

		//gate left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-90, -68, -800)), PxVec3(20, 40, 1));

		//gate left mid
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, -68, -800)), PxVec3(50, 40, 1));

		//gate right mid
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(35, -68, -800)), PxVec3(50, 40, 1));

		//gate right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(90, -68, -800)), PxVec3(20, 40, 1));

		//OBSTACLE 3 - THE SPINNING COLUMNS (WITH CANNONS EITHER SIDE)
		//I have no idea how to make these spin
		//I've added three so there's ample gaps between them to fall down, but allows more than one queue for efficiency
		//also that's what I put in the design and it looked nice
		//may need to make this a different class so it can spin, but I'm making it a floor for the prototype so we can see the level layout more easily
		//if they can't be made to spin, it might be an idea to switch these out for capsules. Should have a similar balance difficulty

		//buffer zone 3 (where contestants respawn on failing the third obstacle, this needs to be sorted on the individual kill plane)

		PxVec3 zone3Position = PxVec3(0, -87, -1050);
		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -1231)), PxVec3(500, 1, 362), zone3Position, respawnSize);
		//left side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -1231)), PxVec3(1, 500, 362), zone3Position, respawnSize);
		//right side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -1231)), PxVec3(1, 500, 362), zone3Position, respawnSize);

		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1050)), PxVec3(200, 1, 100));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -58, -1050)), PxVec3(1, 60, 100));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -58, -1050)), PxVec3(1, 60, 100));

		//this time, the gate is to stop the bowling balls hitting players in the buffer zone. 
		//could also act as a kill plane for the bowling balls
		//top of gate
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -38, -1100)), PxVec3(200, 20, 1));

		//gate left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-90, -68, -1100)), PxVec3(20, 40, 1));

		//gate left mid
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, -68, -1100)), PxVec3(50, 40, 1));

		//gate right mid
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(35, -68, -1100)), PxVec3(50, 40, 1));

		//gate right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(90, -68, -1100)), PxVec3(20, 40, 1));

		//OBSTACLE 4 - RAMPED BOWLING ALLEY
		//so basically it's like that one bit of mario kart, but also indiana jones, takeshi's castle, and probably some other stuff
		//media tends to be surprisingly boulder centric
		//I thought it'd be fun if they were bowling balls rolling down a hill, and you were trying not to get hit 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -331, -1225), PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(200, 1, 300), 1);

		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -325, -1225), PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(1, 12, 310));

		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -325, -1225), PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(1, 12, 310));

		//pegs as obstacles/hiding places for the bowling balls
		//row 1
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-60, 570, -1180), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 570, -1180), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(60, 570, -1180), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

		//row 2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 645, -1235), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 645, -1235), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

		//row 3

		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-60, 720, -1290), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 720, -1290), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(60, 720, -1290), PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

		//buffer zone 4 (where contestants respawn on failing the fourth obstacle, this needs to be sorted on the individual kill plane)
		PxVec3 zone4Position = PxVec3(0, 56, -1411);

		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -1631)), PxVec3(500, 1, 438), zone4Position, respawnSize);
		//left side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -1631)), PxVec3(1, 500, 438), zone4Position, respawnSize);
		//right side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -1631)), PxVec3(1, 500, 438), zone4Position, respawnSize);
		//back kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 100, -1850)), PxVec3(500, 500, 1), zone4Position, respawnSize);


		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 55, -1411)), PxVec3(200, 1, 100));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, 81, -1411)), PxVec3(1, 52, 100));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, 81, -1411)), PxVec3(1, 52, 100));

		//diving boards can be used to give players an advantage in getting further into the blender
		//(if they can stay on, it's not gated to encourage players barging into each other

		//diving board left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-70, 45, -1495)), PxVec3(20, 1, 70), 0.5, 2);

		//diving board centre
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 45, -1495)), PxVec3(20, 1, 70), 0.5, 2);

		//diving board right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(70, 45, -1495)), PxVec3(20, 1, 70), 0.5, 2);

		//blender floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1630.5)), PxVec3(200, 1, 339));

		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, 8, -1630.5)), PxVec3(1, 200, 339));

		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, 8, -1630.5)), PxVec3(1, 200, 339));

		//back wall
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -16.5, -1461)), PxVec3(200, 143, 1));

		//should leave a gap of 20. Enough to get through, but tricky when there's a big old blender shoving everyone around
		//inspired loosely by those windmills on crazy golf
		//front wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-55, -58, -1800)), PxVec3(90, 60, 1));

		//front wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(55, -58, -1800)), PxVec3(90, 60, 1));

		//front wall top
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 23, -1800)), PxVec3(200, 170, 1));

		//roof
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 107, -1580.5)), PxVec3(200, 1, 439));

		//VICTORY PODIUM
		//the room you have to get in after the blender
		//press the button to win
		//(button + podium yet to be added)
		//also want a sliding door that locks players out
		//pressing the button should make all the floors drop out
		//Podium floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1825)), PxVec3(50, 1, 50));

		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-25, -63, -1825)), PxVec3(1, 50, 50));

		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(25, -63, -1825)), PxVec3(1, 50, 50));

		//end wall 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -63, -1850)), PxVec3(50, 50, 1));

		//roof
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -38, -1825)), PxVec3(50, 1, 50));
		break;
	}
}

/* Initialises all game objects, enemies etc */
void LevelCreator::InitGameExamples(LevelState state)
{
	switch (state)
	{
	case LevelState::MENU:
		break;
	case LevelState::LEVEL1:
		GameManager::AddPxPickupToWorld(PxTransform(PxVec3(-20, 20, 0)), 1);
		GameManager::AddPxPlayerToWorld(PxTransform(PxVec3(0, 20, 0)), 1);
		GameManager::AddPxEnemyToWorld(PxTransform(PxVec3(20, 20, 0)), 1);
		break;
	case LevelState::LEVEL2:
		//player added to check this is all a reasonable scale
		GameManager::AddPxPlayerToWorld(PxTransform(PxVec3(0, 1, 0)), 1);
		break;
	}
}

/* This method will initialise any other moveable obstacles we want */
void LevelCreator::InitGameObstacles(LevelState state)
{
	switch (state)
	{
	case LevelState::LEVEL1:
		GameManager::AddPxSphereToWorld(PxTransform(PxVec3(-20, 20, -20)), 2);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 20, -20)), PxVec3(2, 2, 2));
		GameManager::AddPxCapsuleToWorld(PxTransform(PxVec3(20, 20, -20)), 2, 2);
		break;
	case LevelState::LEVEL2:
		//HAVE COMMENTED OUT THE ORIGINAL BEAMS, WILL LEAVE IN IN CASE WE DECIDE TO GO FOR STATIC ONES
		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 200));
		GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 198), PxVec3(0, 0, 1));

		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -98, -900)), PxVec3(20, 20, 200));
		GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(0, -98, -900)), PxVec3(20, 20, 198), PxVec3(0, 0, 1));

		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, -98, -900)), PxVec3(20, 20, 200));
		GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(70, -98, -900)), PxVec3(20, 20, 198), PxVec3(0, 0, 1));

		//cannons
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -80, -850)), PxVec3(7000000, 8500, 0), 10, 10);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -80, -900)), PxVec3(7000000, 8500, 0), 10, 10);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -80, -950)), PxVec3(7000000, 8500, 0), 10, 10);

		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -80, -825)), PxVec3(7000000, 8500, 0), 10, 10);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -80, -875)), PxVec3(7000000, 8500, 0), 10, 10);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -80, -925)), PxVec3(7000000, 8500, 0), 10, 10);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -80, -975)), PxVec3(7000000, 8500, 0), 10, 10);

		//OBSTACLE 5 - THE BLENDER
		//basically, it's an enclosed space with a spinning arm at the bottom to randomise which player actually wins
		//it should be flush with the entrance to the podium room so that the door is reasonably difficult to access unless there's nobody else there
		//again, not sure how to create the arm, it's a moving object, might need another class for this
		//also, it's over a 100m drop to the blender floor, so pls don't put fall damage in blender blade
		GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(0, -78, -1700)), PxVec3(190, 20, 20), PxVec3(0, 1, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-80, 100, -1351)), PxVec3(1, 1, 70000000), 30, 15);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-40, 100, -1351)), PxVec3(1, 1, 70000000), 30, 15);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(0, 100, -1351)), PxVec3(1, 1, 70000000), 30, 15);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(40, 100, -1351)), PxVec3(1, 1, 70000000), 30, 15);
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(80, 100, -1351)), PxVec3(1, 1, 70000000), 30, 15);
		break;
	}
}

/* If in debug mode we can select an object with the cursor, displaying its properties and allowing us to take control */
bool LevelCreator::SelectObject()
{
	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT) && !GameManager::GetLockedObject())
	{
		PxVec3 pos = PhysxConversions::GetVector3(GameManager::GetWorld()->GetMainCamera()->GetPosition());
		PxVec3 dir = PhysxConversions::GetVector3(CollisionDetection::GetMouseDirection(*GameManager::GetWorld()->GetMainCamera()));
		float distance = 10000.0f;
		PxRaycastBuffer hit;

		if (GameManager::GetPhysicsSystem()->GetGScene()->raycast(pos, dir, distance, hit))
		{
			if (GameManager::GetSelectionObject()) {
				GameManager::GetSelectionObject()->SetSelected(false);
				if (GameManager::GetSelectionObject()->GetRenderObject())
					GameManager::GetSelectionObject()->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}

			PxRigidActor* actor = hit.block.actor;
			GameManager::SetSelectionObject(GameManager::GetWorld()->FindObjectFromPhysicsBody(actor));
			GameManager::GetSelectionObject()->SetSelected(true);

			if (GameManager::GetSelectionObject()->GetRenderObject())
				GameManager::GetSelectionObject()->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
			return true;
		}
		return false;
	}

	if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::RIGHT) && !GameManager::GetLockedObject())
	{
		if (GameManager::GetSelectionObject()) {
			GameManager::GetSelectionObject()->SetSelected(false);
			if (GameManager::GetSelectionObject()->GetRenderObject())
				GameManager::GetSelectionObject()->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			GameManager::SetSelectionObject(nullptr);
		}
	}

	/* We can lock the object and move it around */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L))
	{
		if (GameManager::GetSelectionObject())
		{
			if (GameManager::GetLockedObject() == GameManager::GetSelectionObject())
			{
				GameManager::SetCamMode(CameraState::FREE);
				GameManager::SetLockedObject(nullptr);
				Window::GetWindow()->ShowOSPointer(true);
				Window::GetWindow()->LockMouseToWindow(false);
			}
			else
			{
				GameManager::SetCamMode(CameraState::THIRDPERSON);
				GameManager::SetLockedObject(GameManager::GetSelectionObject());
				Window::GetWindow()->ShowOSPointer(false);
				Window::GetWindow()->LockMouseToWindow(true);
			}
			GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetCameraState());
		}
	}
	return false;
}

/* If we've selected an object, we can manipulate it with some key presses */
void LevelCreator::DebugObjectMovement()
{
	if (GameManager::GetSelectionObject()->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
	{
		PxRigidDynamic* body = (PxRigidDynamic*)GameManager::GetSelectionObject()->GetPhysicsObject()->GetPXActor();

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

/* If we have control of an object we can move it around and perform certain actions */
void LevelCreator::LockedObjectMovement(float dt)
{
	/*if (inSelectionMode && selectionObject)
		selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));*/
	Matrix4 view = GameManager::GetWorld()->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	Vector3 charForward = Quaternion(GameManager::GetLockedObject()->GetTransform().GetOrientation()) * Vector3(0, 0, 1);
	float force = 500000.0f;

	if (GameManager::GetLockedObject()->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
	{
		PxRigidDynamic* body = (PxRigidDynamic*)GameManager::GetSelectionObject()->GetPhysicsObject()->GetPXActor();
		body->setLinearDamping(0.4f);

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
			body->addForce(PhysxConversions::GetVector3(fwdAxis) * force * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
			body->addForce(PhysxConversions::GetVector3(-rightAxis) * force * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
			body->addForce(PhysxConversions::GetVector3(-fwdAxis) * force * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
			body->addForce(PhysxConversions::GetVector3(rightAxis) * force * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && GameManager::GetLockedObject()->IsGrounded())
		{
			body->addForce(PhysxConversions::GetVector3(Vector3(0, 1, 0)) * force * 500 * dt, PxForceMode::eIMPULSE);
			//lockedObject->SetGrounded(false);
		}

	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1))
	{
		switch (GameManager::GetCameraState())
		{
		case CameraState::THIRDPERSON:
			GameManager::SetCamMode(CameraState::TOPDOWN);
			break;
		case CameraState::TOPDOWN:
			GameManager::SetCamMode(CameraState::THIRDPERSON);
			break;
		}
		GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetCameraState());
	}
}

void LevelCreator::updateCannons(float dt)
{
	/*for (int i = 0; i < cannons.size(); ++i) {
		cannons[i]->Update(dt);
		if (cannons[i]->getTimeSinceShot() > 5) {
			Vector3 position = cannons[i]->GetTransform().GetPosition();
			WorldCreator::AddPxCannonBallToWorld(PxTransform(*cannons[i]->getCannonBallPosition()), cannons[i], cannons[i]->getShotSize(), cannons[i]->shoot(), cannons[i]->getShotDensity());
		}
	}*/
}

void LevelCreator::updateCannonBalls()
{
	for (int i = 0; i < cannons.size(); ++i)
	{
		for (int j = 0; j < cannons[i]->getShots().size(); ++j)
		{
			if ((cannons[i]->getShots()[j]->getDestroy() || (cannons[i]->getShots()[j]->GetTimeAlive() > cannons[i]->getMaxAlive())))
			{
				GameManager::GetPhysicsSystem()->GetGScene()->removeActor(*cannons[i]->getShots()[j]->getBody());
				cannons[i]->getShots()[j]->SetRenderObject(NULL);
				cannons[i]->getShots()[j]->SetPhysicsObject(NULL);
				GameManager::GetWorld()->RemoveGameObject(cannons[i]->getShots()[j], false);
				cannons[i]->removeShot(cannons[i]->getShots()[j]);
			}
		}
	}
}

void LevelCreator::clearCannons()
{
	for (int i = 0; i < cannons.size(); ++i)
	{
		for (int j = 0; j < cannons[i]->getShots().size(); ++j)
		{
			if (cannons[i]->getShots()[j]->GetRenderObject() != nullptr)
			{

				delete cannons[i]->getShots()[j]->GetRenderObject();

			}
			if (cannons[i]->getShots()[j]->GetPhysicsObject() != nullptr)
			{

				delete cannons[i]->getShots()[j]->GetPhysicsObject();

			}
			GameManager::GetWorld()->RemoveGameObject(cannons[i]->getShots()[j], false);
			cannons[i]->removeShot(cannons[i]->getShots()[j]);
		}
	}

	cannons.clear();
}