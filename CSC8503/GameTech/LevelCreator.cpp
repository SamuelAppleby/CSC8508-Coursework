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
	GameManager::Create(new PxPhysicsSystem(), new GameWorld(), new AudioManager());
	GameManager::LoadAssets();
}

LevelCreator::~LevelCreator() {

}

void LevelCreator::ResetWorld()
{
	GameManager::GetWorld()->ClearAndErase();
	//WorldCreator::GetPhysicsSystem()->ResetPhysics();
}

void LevelCreator::Update(float dt)
{
	GameManager::GetPhysicsSystem()->StepPhysics(dt);
	UpdateLevel(dt);
	GameManager::GetWorld()->UpdateWorld(dt);
	GameManager::GetAudioManager()->UpdateAudio(dt);
	GameManager::GetRenderer()->Update(dt);
	GameManager::GetRenderer()->Render();
	Debug::FlushRenderables(dt);
}

/* Logic for updating level 1 or level 2 */
void LevelCreator::UpdateLevel(float dt)
{
	if (GameManager::GetPlayer()) {
		UpdatePlayer(dt);
	}

	/* Enter debug mode? */
	if (Window::GetKeyboard()->KeyHeld(KeyboardKeys::C) && Window::GetKeyboard()->KeyPressed(KeyboardKeys::H))
	{
		if (GameManager::GetRenderer()->GetUIState() != UIState::DEBUG) 		
			GameManager::GetRenderer()->SetUIState(UIState::DEBUG);
		else 
			GameManager::GetRenderer()->SetUIState(UIState::INGAME);
		GameManager::GetWorld()->SetDebugMode(GameManager::GetRenderer()->GetUIState() == UIState::DEBUG);
		Window::GetWindow()->ShowOSPointer(GameManager::GetRenderer()->GetUIState() == UIState::DEBUG);
		Window::GetWindow()->LockMouseToWindow(GameManager::GetRenderer()->GetUIState() != UIState::DEBUG);
	}

	/* Debug mode selection */
	if (GameManager::GetRenderer()->GetUIState() == UIState::DEBUG)
	{
		UpdateKeys();
		SelectObject();
	}

	/* Change Camera */
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1))
	{
		switch (GameManager::GetWorld()->GetMainCamera()->GetState())
		{
		case CameraState::FREE:
			if (GameManager::GetLevelState() == LevelState::LEVEL1)
				GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::GLOBAL1);
			else
				GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::GLOBAL2);
			break;
		case CameraState::GLOBAL1:
			InitCamera();
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::FREE);
			break;
		case CameraState::GLOBAL2:
			InitCamera();
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::FREE);
			break;
		}
		GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetWorld()->GetMainCamera()->GetState());
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

	else if (GameManager::GetRenderer()->GetUIState() != UIState::DEBUG ||
		GameManager::GetWorld()->GetMainCamera()->GetState() == CameraState::GLOBAL1 || 
		GameManager::GetWorld()->GetMainCamera()->GetState() == CameraState::GLOBAL2)
		GameManager::GetWorld()->GetMainCamera()->UpdateCamera(dt);

	if (GameManager::GetLockedObject())
		LockedObjectMovement(dt);
	else if (GameManager::GetSelectionObject())
	{
		DebugObjectMovement();
	}
}

void LevelCreator::UpdatePlayer(float dt) {
	if (GameManager::GetPlayer()->GetRaycastTimer() <= 0.0f) {
		PxVec3 pos = PhysxConversions::GetVector3(GameManager::GetPlayer()->GetTransform().GetPosition()) + PxVec3(0, -6, 0);
		PxVec3 dir = PxVec3(0, -1, 0);
		float distance = 4.0f;
		PxRaycastBuffer hit;

		if (GameManager::GetPhysicsSystem()->GetGScene()->raycast(pos, dir, distance, hit)) {
			GameObject* obj = GameManager::GetWorld()->FindObjectFromPhysicsBody(hit.block.actor);
			GameManager::GetPlayer()->SetIsGrounded(obj->GetName() == "Floor");
		}
		else {
			GameManager::GetPlayer()->SetIsGrounded(false);
		}
		GameManager::GetPlayer()->SetRaycastTimer(.25f);
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
	if (!GameManager::GetPlayer()) {
		GameManager::GetWorld()->GetMainCamera()->SetPosition(Vector3(0, 50, 80));
		GameManager::GetWorld()->GetMainCamera()->SetYaw(0);
		GameManager::GetWorld()->GetMainCamera()->SetPitch(0);
		GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::FREE);
	}
	else {
		GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
	}
}

/* Initialise all the elements contained within the world */
void LevelCreator::InitWorld(LevelState state)
{
	InitCamera();
	InitFloors(state);
	InitGameExamples(state);
	InitGameObstacles(state);
}

void LevelCreator::InitPlayer(const PxTransform& t, const PxReal scale) {
	GameManager::SetPlayer(GameManager::AddPxPlayerToWorld(t, scale));
	GameManager::SetLockedObject(GameManager::GetPlayer());
	GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
}

/* Place all the levels solid floors */
void LevelCreator::InitFloors(LevelState state)
{
	Vector3 respawnSize;
	PxVec3 zone1Position, zone2Position, zone3Position, zone4Position;
	switch (state)
	{
	case LevelState::LEVEL1:
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -20, 0)), PxVec3(100, 1, 100));
		break;
	case LevelState::LEVEL2:
		//fyi, the buffer zones go between obstacles. This is to give the player time to think so it's not just all one muscle memory dash
		//(that way, it also allows time for other players to catch up and makes each individual obstacle more chaotic, so it's a double win)
		//first we'll do the floors
		//starting zone
		//floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 0, 0     )*2), PxVec3(200, 1, 100));
		//back wall													   
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 4.5, 50  )*2), PxVec3(200, 10, 1));
		//side wall left											   
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, 4.5, 0)*2), PxVec3(1, 10, 100));
		//side wall right											   
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, 4.5, 0 )*2), PxVec3(1, 10, 100));

		//OBSTACLE 0.5 - THE RAMP DOWN
		//after a short platform for players to build up some momentum, the players are sent down a ramp to build up speed
		//it should be slippery as well (possibly do some stuff with coefficients?) so that players don't have as much control

		//first ramp down (maybe some stuff to make it slippery after, it's designed to be like the total wipe out slide
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -44.122, -192.85) * 2, PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(200, 1, 300), 0, 0,TextureState::ICE);
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -40.3, -193.8) * 2, PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(1, 10, 305));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -40.3, -193.8) * 2, PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(1, 10, 305));

		//buffer zone 1 (where contestants respawn on failing the first obstacle, this needs to be sorted on the individual kill plane)
		respawnSize = Vector3(180, 0, 80);
		zone1Position = PxVec3(0, -87, -384);
		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -325  )*2), PxVec3(500, 1, 850), zone1Position, respawnSize);
		//back kill plane													  
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 100, 100    )*2), PxVec3(500, 500, 1), zone1Position, respawnSize);
		//left side kill plane												  
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -325)*2), PxVec3(1, 500, 850), zone1Position, respawnSize);
		//right side kill plane												  
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -325 )*2), PxVec3(1, 500, 850), zone1Position, respawnSize);
																			  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -384       )*2), PxVec3(200, 1, 100));
		//side wall left													  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -84, -384     )*2), PxVec3(1, 10, 100));
		//side wall right													  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -84, -384    )*2), PxVec3(1, 10, 100));

		//OBSTACLE 1 - THE STEPPING STONE TRAMPOLINES
		//supposed to be bouncy floors/stepping stones type things, think like a combo of the trampolines from the end of splash mountain in total wipeout,
		//and the stepping stones from takeshi's castle
		//may need to add more if the jump isn't far enough
		//honestly not sure on some of these values, there may not be enough stepping stones, so we should decide if more are needed after a little testing
		//front row
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-60, -88, -470)*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -480  )*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, -88, -475 )*2), PxVec3(30, 1, 30));
		//next row														 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-40, -88, -550)*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(10, -88, -540 )*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(60, -88, -520 )*2), PxVec3(30, 1, 30));
		//last row														 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-65, -88, -610)*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-20, -88, -620)*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(20, -88, -640 )*2), PxVec3(30, 1, 30));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(70, -88, -600 )*2), PxVec3(30, 1, 30));

		//buffer zone 2 (where contestants respawn on failing the second obstacle, this needs to be sorted on the individual kill plane)
		zone2Position = PxVec3(0, -87, -750);

		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -900  )*2), PxVec3(500, 1, 300), zone2Position, respawnSize);
		//left side kill plane												  
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -900)*2), PxVec3(1, 500, 300), zone2Position, respawnSize);
		//right side kill plane												  
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -900 )*2), PxVec3(1, 500, 300), zone2Position, respawnSize);
																			  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -750       )*2), PxVec3(200, 1, 100));
		//side wall left													  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -58, -750     )*2), PxVec3(1, 60, 100));
		//side wall right													  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -58, -750    )*2), PxVec3(1, 60, 100));

		//gate for buffer zone 2 (prevents players jumping ahead on the spinning platforms)
		//top of gate, may need to give this some height if there are problems getting onto platforms from jumping
		//have made the front walls taller to accommodate this, so only the top piece (below) needs to be adjusted
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -48, -800  )*2), PxVec3(200, 40, 1));
																		 
		//gate left														 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-90, -68, -800)*2), PxVec3(20, 40, 1));
																		 
		//gate left mid													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, -68, -800)*2), PxVec3(50, 40, 1));
																		 
		//gate right mid												 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(35, -68, -800 )*2), PxVec3(50, 40, 1));
																		 
		//gate right													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(90, -68, -800 )*2), PxVec3(20, 40, 1));

		//OBSTACLE 3 - THE SPINNING COLUMNS (WITH CANNONS EITHER SIDE)
		//I have no idea how to make these spin
		//I've added three so there's ample gaps between them to fall down, but allows more than one queue for efficiency
		//also that's what I put in the design and it looked nice
		//may need to make this a different class so it can spin, but I'm making it a floor for the prototype so we can see the level layout more easily
		//if they can't be made to spin, it might be an idea to switch these out for capsules. Should have a similar balance difficulty

		//buffer zone 3 (where contestants respawn on failing the third obstacle, this needs to be sorted on the individual kill plane)

		zone3Position = PxVec3(0, -87, -1050);
		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -1231  )*2), PxVec3(500, 1, 362), zone3Position, respawnSize);
		//left side kill plane												   
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -1231)*2), PxVec3(1, 500, 362), zone3Position, respawnSize);
		//right side kill plane												   
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -1231 )*2), PxVec3(1, 500, 362), zone3Position, respawnSize);
																			   
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1050       )*2), PxVec3(200, 1, 100));
		//side wall left													   
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -58, -1050     )*2), PxVec3(1, 60, 100));
		//side wall right													   
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -58, -1050    )*2), PxVec3(1, 60, 100));

		//this time, the gate is to stop the bowling balls hitting players in the buffer zone. 
		//could also act as a kill plane for the bowling balls
		//top of gate
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -38, -1100  )*2), PxVec3(200, 20, 1));
																		  
		//gate left														  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-90, -68, -1100)*2), PxVec3(20, 40, 1));
																		  
		//gate left mid													  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, -68, -1100)*2), PxVec3(50, 40, 1));
																		  
		//gate right mid												  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(35, -68, -1100 )*2), PxVec3(50, 40, 1));
																		  
		//gate right													  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(90, -68, -1100 )*2), PxVec3(20, 40, 1));

		//OBSTACLE 4 - RAMPED BOWLING ALLEY
		//so basically it's like that one bit of mario kart, but also indiana jones, takeshi's castle, and probably some other stuff
		//media tends to be surprisingly boulder centric
		//I thought it'd be fun if they were bowling balls rolling down a hill, and you were trying not to get hit 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -17, -1232)*2, PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(200, 1, 300), 1);

		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, -17, -1225) * 2, PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(1, 12, 310));

		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, -17, -1225) * 2, PxQuat(0.5, PxVec3(1, 0, 0))), PxVec3(1, 12, 310));

		//pegs as obstacles/hiding places for the bowling balls
		//row 1
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-60, -20, -1180) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -20, -1180) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(60, -20, -1180) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

		//row 2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 10, -1235) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 10, -1235) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

		//row 3
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-60, 40, -1290) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 40, -1290) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(60, 40, -1290) * 2, PxQuat(-0.95, PxVec3(1, 0, 0))), PxVec3(10, 10, 50));

		//buffer zone 4 (where contestants respawn on failing the fourth obstacle, this needs to be sorted on the individual kill plane)
		zone4Position = PxVec3(0, 56, -1411);

		//bottom kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -150, -1631) * 2), PxVec3(500, 1, 438), zone4Position, respawnSize);
		//left side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(-250, 100, -1631) * 2), PxVec3(1, 500, 438), zone4Position, respawnSize);
		//right side kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(250, 100, -1631) * 2), PxVec3(1, 500, 438), zone4Position, respawnSize);
		//back kill plane
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 100, -1850) * 2), PxVec3(500, 500, 1), zone4Position, respawnSize);

		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 55, -1411) * 2), PxVec3(200, 1, 100));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, 81, -1411) * 2), PxVec3(1, 52, 100));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, 81, -1411) * 2), PxVec3(1, 52, 100));

		//diving boards can be used to give players an advantage in getting further into the blender
		//(if they can stay on, it's not gated to encourage players barging into each other

		//diving board left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-70, 45, -1495) * 2), PxVec3(20, 1, 70), 0.5, 2);

		//diving board centre
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 45, -1495) * 2), PxVec3(20, 1, 70), 0.5, 2);

		//diving board right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(70, 45, -1495) * 2), PxVec3(20, 1, 70), 0.5, 2);

		//blender floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1630.5) * 2), PxVec3(200, 1, 339));

		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-100, 8, -1630.5) * 2), PxVec3(1, 200, 339));

		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(100, 8, -1630.5) * 2), PxVec3(1, 200, 339));

		//back wall
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -16.5, -1461) * 2), PxVec3(200, 143, 1));

		//should leave a gap of 20. Enough to get through, but tricky when there's a big old blender shoving everyone around
		//inspired loosely by those windmills on crazy golf
		//front wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-55, -58, -1800) * 2), PxVec3(90, 60, 1));

		//front wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(55, -58, -1800) * 2), PxVec3(90, 60, 1));

		//front wall top
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 23, -1800) * 2), PxVec3(200, 170, 1));

		//roof
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 107, -1580.5) * 2), PxVec3(200, 1, 439));

		//VICTORY PODIUM
		//the room you have to get in after the blender
		//press the button to win
		//(button + podium yet to be added)
		//also want a sliding door that locks players out
		//pressing the button should make all the floors drop out
		//Podium floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -88, -1825) * 2), PxVec3(50, 1, 50));

		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-25, -63, -1825) * 2), PxVec3(1, 50, 50));

		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(25, -63, -1825) * 2), PxVec3(1, 50, 50));

		//end wall 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -63, -1850) * 2), PxVec3(50, 50, 1));

		//roof
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -38, -1825) * 2), PxVec3(50, 1, 50));
		break;

	case LevelState::LEVEL3:
		//floor
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 0, -25) * 2), PxVec3(100, 1, 150));
		
		//back wall															 *2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 4.5, 50        )*2), PxVec3(100, 10, 1));
		//side wall left													 *2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-50, 4.5, -25     )*2), PxVec3(1, 10, 150));
		//side wall right													 *2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, 4.5, -25      )*2), PxVec3(1, 10, 150));
		//Climping wall														 *2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 75, -100       )*2), PxVec3(100, 150, 1));
		//Wall Trambolines													 *2
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 10, -85        )*2), PxVec3(25, 1, 25), 0.5F, 5.0F);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 20, -85       )*2), PxVec3(25, 1, 25), 0.5F, 5.0F);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 20, -85      )*2), PxVec3(25, 1, 25), 0.5F, 5.0F);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 30, -85        )*2), PxVec3(25, 1, 25), 0.5F, 5.0F);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 45, -85       )*2), PxVec3(25, 1, 25), 0.5F, 5.0F);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 45, -85      )*2), PxVec3(25, 1, 25), 0.5F, 5.0F);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 60, -85        )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 75, -85       )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 75, -85      )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 85, -85        )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 100, -85      )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 100, -85     )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 110, -85       )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(30, 120, -85      )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-30, 120, -85     )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);
																			 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 135, -85       )*2), PxVec3(25, 1, 25), 0.5F, 5.0f);

		//buffer zone 1 (where contestants respawn on failing the first obstacle, this needs to be sorted on the individual kill plane)
		respawnSize = Vector3(100, 0, 45);
		zone1Position = PxVec3(0, 10, 0);

		//Kill PLlanes for out of bounce 
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -20, -25)*2), PxVec3(500, 1, 300), zone1Position, respawnSize,false);

		//Kill PLlanes for out of bounce 
																			 
		//2nd floor															 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 150, -124.5    )*2), PxVec3(100, 1, 50));
		//side wall left													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, 154.5, -124.5 )*2), PxVec3(1, 10, 50));
		//side wall right													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-50, 154.5, -124.5)*2), PxVec3(1, 10, 50));
																			 
		//BRIDGE 															 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 150, -199.5    )*2), PxVec3(20, 1, 100));
																			 
																			 
		//2nd Floor, after bridge											 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 150, -270      )*2), PxVec3(100, 1, 50));
		//side wall left													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, 154.5, -270   )*2), PxVec3(1, 10, 50));
		//side wall right													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-50, 154.5, -270  )*2), PxVec3(1, 10, 50));
																			 
		//right platform													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(36.5, 150, -315   )*2), PxVec3(25, 1, 25));
																			 
		//left platform														 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-36.5, 150, -315  )*2), PxVec3(25, 1, 25));
																		
		//Center platform													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 150, -360      )*2), PxVec3(50, 1, 50));
																			 
		//right platform													 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(36.5, 150, -410   )*2), PxVec3(25, 1, 25));
																			 
		//left platform														 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-36.5, 150, -410  )*2), PxVec3(25, 1, 25));
																			 
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, -20, -25)*2), PxVec3(500, 1, 300), zone1Position, respawnSize);
		//GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, -20, -25)), PxVec3(500, 1, 300));


		
		//buffer zone 2
		zone2Position = PxVec3(0, 153, -112);
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 90, -187)*2), PxVec3(500, 1, 170), zone2Position, respawnSize);
		//platform after blender platforms
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 150, -460)*2), PxVec3(100, 1, 50));
		//side wall left
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, 154.5, -460)*2), PxVec3(1, 10, 50));
		//side wall right
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-50, 154.5, -460)*2), PxVec3(1, 10, 50));

        //buffer zone 3
		zone3Position = PxVec3(0, 153, -270);
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 90, -370)*2), PxVec3(500, 1, 190), zone3Position, respawnSize);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 99, -272)*2), PxVec3(500, 100, 1), 0.5F, 0.1F, TextureState::INVISIBLE);
		//slippery ramp
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 120, -580) * 2, PxQuat(-0.3, PxVec3(1, 0, 0))), PxVec3(100, 1, 200), 0, 0,TextureState::ICE);

		//buffer zone 4
		zone4Position = PxVec3(0, 153, -460);
		GameManager::AddPxKillPlaneToWorld(PxTransform(PxVec3(0, 70, -572)*2), PxVec3(500, 1, 210), zone4Position, respawnSize);
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 99, -470)*2), PxVec3(500, 100, 1),0.5F,0.1F,TextureState::INVISIBLE);



		//Floor after ramp
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(0, 91, -825    )*2), PxVec3(100, 1, 300));
		//side wall left												  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(50, 95.5, -825 )*2), PxVec3(1, 10, 300));
		//side wall right												  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-50, 95.5, -825)*2), PxVec3(1, 10, 300));
																		  

		//Pillars
		for (int i = 0; i <= 15; i++) {
			GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-44 + (i * 6), 95.5, -775)*2), PxVec3(2, 10, 2));
			GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-44 + (i * 6), 95.5, -825)*2), PxVec3(2, 10, 2));
			GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-44 + (i * 6), 95.5, -875)*2), PxVec3(2, 10, 2));
		}

		//First row of wall obstacle 
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-47, 95.5, -775  )*2), PxVec3(4, 10, 2));//side wall blocking remaining gap
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-41, 94.5, -775  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, 94.5, -775  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-29, 94.5, -775  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-17, 94.5, -775  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-11, 94.5, -775  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-5, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(1, 94.5, -775    )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(7, 94.5, -775    )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(13, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(19, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(25, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(31, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(37, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(43, 94.5, -775   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(48, 95.5, -775   )*2), PxVec3(4, 10, 2));//side wall blocking remaining gap

		//Middle of wall obstacle
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-47, 95.5, -825)*2), PxVec3(4, 10, 2));//side wall blocking remaining gap
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-41, 94.5, -825)*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, 94.5, -825)*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-29, 94.5, -825)*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-23, 94.5, -825)*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-17, 94.5, -825)*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-11, 94.5, -825)*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-5, 94.5, -825)*2), PxVec3(4, 6, 2));
		//gap in wall
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(7, 94.5, -825   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(13, 94.5, -825  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(19, 94.5, -825  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(25, 94.5, -825  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(31, 94.5, -825  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(37, 94.5, -825  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(43, 94.5, -825  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(48, 95.5, -825  )*2), PxVec3(4, 10, 2));//side wall blocking remaining gap
																		   
		//Last row of wall obstacle 									  
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-47, 95.5, -875 )*2), PxVec3(4, 10, 2));//side wall blocking remaining gap
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-41, 94.5, -875 )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-35, 94.5, -875 )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-29, 94.5, -875 )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-23, 94.5, -875 )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-17, 94.5, -875 )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-11, 94.5, -875 )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(-5, 94.5, -875  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(1, 94.5, -875   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(7, 94.5, -875   )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(13, 94.5, -875  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(19, 94.5, -875  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(25, 94.5, -875  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(31, 94.5, -875  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(37, 94.5, -875  )*2), PxVec3(4, 6, 2));
		GameManager::AddPxFloorToWorld(PxTransform(PxVec3(48, 95.5, -875  )*2), PxVec3(4, 10, 2));//side wall blocking remaining gap
		break;															  
	}
}

/* Initialises all game objects, enemies etc */
void LevelCreator::InitGameExamples(LevelState state)
{
	switch (state)
	{
	case LevelState::LEVEL1:
		InitPlayer(PxTransform(PxVec3(0, 20, 0)), 1);
		GameManager::AddPxCoinToWorld(PxTransform(PxVec3(-20, 5, 0)), 3);
		GameManager::AddPxEnemyToWorld(PxTransform(PxVec3(20, 20, 0)), 1);
		break;
	case LevelState::LEVEL2:
		//player added to check this is all a reasonable scale
		InitPlayer(PxTransform(PxVec3(0, 10, 0)), 1);
		break;
	case LevelState::LEVEL3:
		InitPlayer(PxTransform(PxVec3(0, 10, 0)), 1);
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
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(-70, -98, -900)*2, PxQuat(1.5701, PxVec3(1, 0, 0))), 
			20, 100, PxVec3(0, 0,1));															
		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -98, -900      )*2), PxVec3(20, 20, 200));
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, -98, -900) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))), 
			20, 100, PxVec3(0, 0, 1));
		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, -98, -900     )*2), PxVec3(20, 20, 200));
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(70, -98, -900) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))), 
			20, 100, PxVec3(0, 0, 1));

		//cannons																
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -850     )*2), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -900     )*2), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -950     )*2), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
																				
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -825      )*2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -875      )*2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -925      )*2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -975      )*2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));

		//OBSTACLE 5 - THE BLENDER
		//basically, it's an enclosed space with a spinning arm at the bottom to randomise which player actually wins
		//it should be flush with the entrance to the podium room so that the door is reasonably difficult to access unless there's nobody else there
		//again, not sure how to create the arm, it's a moving object, might need another class for this
		//also, it's over a 100m drop to the blender floor, so pls don't put fall damage in blender blade
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, -78, -1705) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))),
			20, 80, PxVec3(0, 2, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-80, 100, -1351     )*2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0,25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-40, 100, -1351     )*2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(0, 100, -1351       )*2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(40, 100, -1351      )*2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(80, 100, -1351      )*2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));


		for (int i = 0; i < 30; i++)
		{
			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)*2), 20));
		}

		/*
		for (int i = 0; i < 5; i++)
		{
			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 20));
		}

		for (int i = 0; i < 7; i++)
		{
			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 10));
		}

		for (int i = 0; i < 5; i++)
		{
			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 20));
		}
		*/
		//PxTransform t = PxTransform(PxVec3(0, 50, 0));

		//GameManager::AddPxCannonBallToWorld(t);
		break;

	case LevelState::LEVEL3:
		//OBSTACLE 1
		//Rotating pillars
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 177, -180) *2 ), 10, 25, PxVec3(0, 0, 1));
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 177, -200) *2 ), 10, 25, PxVec3(0, 0, 1));
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 177, -220) *2 ), 10, 25, PxVec3(0, 0, 1));
	
		//OBSTACLE2 
		//Jumping platforms with blenders
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(36.5, 152.5, -315) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))),
			3, 12, PxVec3(0, 1, 0));

		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(-36.5, 152.5, -315) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 
			3, 12, PxVec3(0, 1, 0));

		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 152.5, -360) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 
			3, 24, PxVec3(0, 1, 0));
				
		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(36.5, 152.5, -410) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 
			3, 12, PxVec3(0, 1, 0));

		GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(-36.5, 152.5, -410) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 
			3, 12, PxVec3(0, 1, 0));

		//OBSTACLE 3
		//bouncing sticks on the slide 
		for (int i = 0; i <= 7; i++) {
			GameManager::AddBounceSticks(PxTransform(PxVec3(-35+(i*10), 140, -522)*2), 2, 2, 10.0F, 0.5F, 1.0F);
			GameManager::AddBounceSticks(PxTransform(PxVec3(-35+(i*10), 116, -602)*2), 2, 2, 10.0F, 0.5F, 1.0F);
		}

		for (int i = 0; i <= 8; i++) {
			GameManager::AddBounceSticks(PxTransform(PxVec3(-40 + (i * 10), 128, -562)*2), 2, 2, 10.0F, 0.5F, 1.0F);
			GameManager::AddBounceSticks(PxTransform(PxVec3(-40 + (i * 10), 104, -642)*2), 2, 2, 10.0F, 0.5F, 1.0F);
		}
		
		//OBSTACLE 4
		//Running through walls
		//cubes		
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-22, 93, -775) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-22, 95, -775) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-22, 97, -775) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-24, 93, -775) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-24, 95, -775) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-24, 97, -775) * 2), PxVec3(2, 2, 2), 1.0F);

		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 93, -825) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 95, -825) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 97, -825) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(2, 93, -825) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(2, 95, -825) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(2, 97, -825) * 2), PxVec3(2, 2, 2), 1.0F);

		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(42, 93, -875) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(42, 95, -875) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(42, 97, -875) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(44, 93, -875) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(44, 95, -875) * 2), PxVec3(2, 2, 2), 1.0F);
		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(44, 97, -875) * 2), PxVec3(2, 2, 2), 1.0F);
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
			if (GameManager::GetSelectionObject())
			{
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
		if (GameManager::GetSelectionObject())
		{
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
				GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::FREE);
				GameManager::SetLockedObject(nullptr);
				Window::GetWindow()->ShowOSPointer(true);
				Window::GetWindow()->LockMouseToWindow(false);
			}
			else
			{
				GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
				GameManager::SetLockedObject(GameManager::GetSelectionObject());
				Window::GetWindow()->ShowOSPointer(false);
				Window::GetWindow()->LockMouseToWindow(true);
			}
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
	/*Matrix4 view = GameManager::GetWorld()->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	Vector3 charForward = Quaternion(GameManager::GetLockedObject()->GetTransform().GetOrientation()) * Vector3(0, 0, 1);
	float force = 500000.0f;

	if (GameManager::GetLockedObject()->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
	{
		PxRigidDynamic* body = (PxRigidDynamic*)GameManager::GetLockedObject()->GetPhysicsObject()->GetPXActor();
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
		}

	}*/

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::NUM1))
	{
		switch (GameManager::GetWorld()->GetMainCamera()->GetState())
		{
		case CameraState::THIRDPERSON:
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::TOPDOWN);
			break;
		case CameraState::TOPDOWN:
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
			break;
		}
	}
}