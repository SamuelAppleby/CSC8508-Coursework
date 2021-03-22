#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#define COLLISION_MSG 30

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame() {
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket = 0.0f;
	packetsToSnapshot = 0;

	GameManager::GetRenderer()->SetNetworkedGame(this);
}

NetworkedGame::~NetworkedGame() {
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::ResetWorld() {
	networkObjects.clear();
	serverPlayers.clear();
	stateIDs.clear();

	LevelCreator::ResetWorld();
}

void NetworkedGame::StartAsServer(LevelState state, string playerName) {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Received_State, this);

	//std::cout << "Starting as server." << std::endl;

	InitWorld(state);
	localPlayer = SpawnPlayer(-1);
	localPlayer->SetPlayerName(playerName);
	localPlayer->SetHost();

	GameManager::SetPlayer(localPlayer);
	GameManager::SetLockedObject(localPlayer);
	GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);

	/*GameManager::SetSelectionObject(localPlayer);
	localPlayer->SetSelected(true);
	GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
	GameManager::SetLockedObject(localPlayer);
	GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetWorld()->GetMainCamera()->GetState());*/
}

void NetworkedGame::StartAsClient(LevelState state, string playerName, string ip) {
	thisClient = new GameClient();
	thisClient->Connect(ip, NetworkBase::GetDefaultPort());

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);

	//std::cout << "Starting as client." << std::endl;

	localPlayerName = playerName;

	clientState = state;
}

void NetworkedGame::Update(float dt) {
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / 120.0f; //120hz server/client update
	}

	if (thisServer) {
		GameManager::GetPhysicsSystem()->StepPhysics(dt);
	}
	
	UpdateLevel(dt);
	GameManager::GetWorld()->UpdateWorld(dt);
	GameManager::GetAudioManager()->UpdateAudio(dt);
	GameManager::GetRenderer()->Update(dt);
	GameManager::GetRenderer()->Render();
	Debug::FlushRenderables(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	thisServer->UpdateServer(dt);

	if (serverPlayers.size() < thisServer->players.size()) {
		for (auto i : thisServer->players) {
			std::map<int, NetworkPlayer*>::iterator it;
			it = serverPlayers.find(i.first);

			if (it == serverPlayers.end()) {
				serverPlayers.insert(std::pair<int, NetworkPlayer*>(i.first, SpawnPlayer(i.first)));
				stateIDs.insert(std::pair<int, int>(i.first, 0));
			}
		}
	}
	else if (serverPlayers.size() > thisServer->players.size()) {
		for (auto i = serverPlayers.begin(); i != serverPlayers.end(); ) {
		//for (auto i : serverPlayers) {
			std::map<int, ENetPeer*>::iterator it;
			it = thisServer->players.find((*i).first);

			if (it == thisServer->players.end()) {
				(*i).second->Disconnect();
				stateIDs.erase((*i).first);
				i = serverPlayers.erase(i);
			}
			else {
				i++;
			}
		}
	}

	if (stateIDs.size() > 0) {
		UpdateMinimumState();
	}

	packetsToSnapshot--;
	if (packetsToSnapshot < 0) {
		BroadcastSnapshot(false);
		packetsToSnapshot = 5;
	}
	else {
		BroadcastSnapshot(true);
	}
}

void NetworkedGame::UpdateAsClient(float dt) {
	ClientPacket newPacket;
	thisClient->UpdateClient(dt);

	Camera* c = GameManager::GetWorld()->GetMainCamera();
	Matrix4 view = c->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	newPacket.rightAxis = Vector3(camWorld.GetColumn(0));
	newPacket.cameraYaw = c->GetYaw();

	if (localPlayer) {
		newPacket.playerName = localPlayer->GetPlayerName();

		if (localPlayer->HasFinished()) {
			newPacket.finishTime = localPlayer->GetFinishTime();
		}

		UIState ui = GameManager::GetRenderer()->GetUIState();

		if (ui == UIState::INGAME || ui == UIState::SCOREBOARD) {
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
				newPacket.buttonstates[0] = 1;
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
				newPacket.buttonstates[1] = 1;
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
				newPacket.buttonstates[2] = 1;
			if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
				newPacket.buttonstates[3] = 1;
			if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
				newPacket.buttonstates[4] = 1;
			}
		}
	}

	newPacket.lastID = thisClient->lastPacketID;
	thisClient->SendPacket(newPacket);
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
	/*std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	GameManager::GetWorld()->GetObjectIterators(first, last);*/

	for (auto p : serverPlayers) {
		//for (auto i = first; i != last; ++i) { // Change to loop through networkObjects?
		for (auto o : networkObjects) {
			/*NetworkObject* o = (*i)->GetNetworkObject();
			if (!o) {
				continue;
			}*/

			int playerState = stateIDs.at(p.first);
			GamePacket* newPacket = nullptr;
			if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
				thisServer->SendPacketToPeer(p.first, *newPacket);
				delete newPacket;
			}
		}
	}
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = min(minID, i.second);
		maxID = max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	GameManager::GetWorld()->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}

	//std::cout << "Server min state: " << minID << std::endl;
}

void NetworkedGame::UpdatePlayer(NetworkPlayer* player, float dt) {
	if (player->GetRaycastTimer() <= 0.0f) {
		PxVec3 pos = PhysxConversions::GetVector3(player->GetTransform().GetPosition()) + PxVec3(0, -6, 0);
		PxVec3 dir = PxVec3(0, -1, 0);
		float distance = 4.0f;
		PxRaycastBuffer hit;

		if (GameManager::GetPhysicsSystem()->GetGScene()->raycast(pos, dir, distance, hit)) {
			GameObject* obj = GameManager::GetWorld()->FindObjectFromPhysicsBody(hit.block.actor);
			player->SetIsGrounded(obj->GetName() == "Floor");
		}
		else {
			player->SetIsGrounded(false);
		}
		player->SetRaycastTimer(.25f);
	}
}

NetworkPlayer* NetworkedGame::SpawnPlayer(int playerNum) {
	NetworkPlayer* p = GameManager::AddPxNetworkPlayerToWorld(PxTransform(PxVec3(-5 * playerNum, 1, 0)), 1, this, playerNum);
	NetworkObject* n = new NetworkObject(*p, levelNetworkObjectsCount + playerNum + 1);
	networkObjects.emplace_back(n);

	return p;
}

void NetworkedGame::AddLevelNetworkObject(GameObject* object) {
	networkObjects.emplace_back(new NetworkObject(*object, networkObjects.size()));
	levelNetworkObjectsCount++;
}

void NetworkedGame::InitWorld(LevelState state) {
	InitCamera();
	//GameManager::SetLevelState(state);
	InitFloors(state);
	InitGameObstacles(state);
}

void NetworkedGame::InitGameObstacles(LevelState state) {
	switch (state)
	{
	case LevelState::LEVEL1:
		AddLevelNetworkObject(GameManager::AddPxCoinToWorld(PxTransform(PxVec3(40, 5, -20)), 2));
		AddLevelNetworkObject(GameManager::AddPxSphereToWorld(PxTransform(PxVec3(-20, 20, -20)), 2));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 20, -20)), PxVec3(2, 2, 2)));
		AddLevelNetworkObject(GameManager::AddPxCapsuleToWorld(PxTransform(PxVec3(20, 20, -20)), 2, 2));
		break;
	case LevelState::LEVEL2:
		//HAVE COMMENTED OUT THE ORIGINAL BEAMS, WILL LEAVE IN IN CASE WE DECIDE TO GO FOR STATIC ONES
		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 200));
		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(-70, -98, -900) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))), 20, 100, PxVec3(0, 0, 1)));

		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -98, -900      )*2), PxVec3(20, 20, 200));
		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, -98, -900) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))), 20, 100, PxVec3(0, 0, 1)));

		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, -98, -900     )*2), PxVec3(20, 20, 200));
		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(70, -98, -900) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))), 20, 100, PxVec3(0, 0, 1)));

		//cannons																
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -850) * 2), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -900) * 2), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -950) * 2), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));

		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -825) * 2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -875) * 2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -925) * 2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -975) * 2), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));

		//OBSTACLE 5 - THE BLENDER
		//basically, it's an enclosed space with a spinning arm at the bottom to randomise which player actually wins
		//it should be flush with the entrance to the podium room so that the door is reasonably difficult to access unless there's nobody else there
		//again, not sure how to create the arm, it's a moving object, might need another class for this
		//also, it's over a 100m drop to the blender floor, so pls don't put fall damage in blender blade
		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, -78, -1705) * 2, PxQuat(1.5701, PxVec3(1, 0, 0))), 20, 80, PxVec3(0, 2, 0)));


		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-80, 100, -1351) * 2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-40, 100, -1351) * 2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(0, 100, -1351) * 2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(40, 100, -1351) * 2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(80, 100, -1351) * 2), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));

		for (int i = 0; i < 30; i++)
		{
			Cannonball* c = GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000) * 2), 20);
			AddLevelNetworkObject(c);
			GameManager::GetObstacles()->cannons.push_back(c);
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
		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 177, -180) * 2), 10, 25, PxVec3(0, 0, 1)));

		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 177, -200) * 2), 10, 25, PxVec3(0, 0, 1)));

		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 177, -220) * 2), 10, 25, PxVec3(0, 0, 1)));

		//OBSTACLE2 
		//Jumping platforms with blenders
		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(36.5, 152.5, -315) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 3, 12, PxVec3(0, 1, 0)));

		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(-36.5, 152.5, -315) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 3, 12, PxVec3(0, 1, 0)));

		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(0, 152.5, -360) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 3, 24, PxVec3(0, 1, 0)));

		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(36.5, 152.5, -410) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))), 3, 12, PxVec3(0, 1, 0)));

		AddLevelNetworkObject(GameManager::AddPxRotatingCylinderToWorld(PxTransform(PxVec3(-36.5, 152.5, -410) * 2, PxQuat(1.5701, PxVec3(0, 0, 1))),	3, 12, PxVec3(0, 1, 0)));

		//OBSTACLE 3
		//bouncing sticks on the slide 
		for (int i = 0; i <= 7; i++) {
			GameManager::AddBounceSticks(PxTransform(PxVec3(-35 + (i * 10), 140, -522) * 2), 2, 2, 10.0F, 0.5F, 1.0F);
			GameManager::AddBounceSticks(PxTransform(PxVec3(-35 + (i * 10), 116, -602) * 2), 2, 2, 10.0F, 0.5F, 1.0F);
		}

		for (int i = 0; i <= 8; i++) {
			GameManager::AddBounceSticks(PxTransform(PxVec3(-40 + (i * 10), 128, -562) * 2), 2, 2, 10.0F, 0.5F, 1.0F);
			GameManager::AddBounceSticks(PxTransform(PxVec3(-40 + (i * 10), 104, -642) * 2), 2, 2, 10.0F, 0.5F, 1.0F);
		}

		//OBSTACLE 4
		//Running through walls
		//cubes		
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-22, 93, -775) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-22, 95, -775) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-22, 97, -775) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-24, 93, -775) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-24, 95, -775) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(-24, 97, -775) * 2), PxVec3(2, 2, 2), 1.0F));

		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 93, -825) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 95, -825) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 97, -825) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(2, 93, -825) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(2, 95, -825) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(2, 97, -825) * 2), PxVec3(2, 2, 2), 1.0F));

		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(42, 93, -875) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(42, 95, -875) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(42, 97, -875) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(44, 93, -875) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(44, 95, -875) * 2), PxVec3(2, 2, 2), 1.0F));
		AddLevelNetworkObject(GameManager::AddPxCubeToWorld(PxTransform(PxVec3(44, 97, -875) * 2), PxVec3(2, 2, 2), 1.0F));

		break;
	}
}

void NetworkedGame::ReceivePacket(float dt, int type, GamePacket* payload, int source) {
	//std::cout << "SOME SORT OF PACKET RECEIVED" << std::endl;
	if (type == Received_State) {	//Server version of the game receives these from players
		//std::cout << "Server: Received packet from client " << source << "." << std::endl;
		ClientPacket* realPacket = (ClientPacket*)payload;

		std::map<int, int>::iterator it;
		it = stateIDs.find(source);

		if (it != stateIDs.end()) {
			it->second = realPacket->lastID;
		}

		if (serverPlayers.size() >= (source + 1)) {
			Vector3 rightAxis = realPacket->rightAxis;

			Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
			fwdAxis.y = 0.0f;
			fwdAxis.Normalise();
			float force = 1000000.0f;

			NetworkPlayer* player = serverPlayers.at(source);

			if (realPacket->playerName != "") {
				player->SetPlayerName(realPacket->playerName);
			}

			if (realPacket->finishTime > 0) {
				player->SetFinishTime(realPacket->finishTime);
				player->SetFinished(true);
			}

			UpdatePlayer(player, dt);

			if (player->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
			{
				PxRigidDynamic* body = (PxRigidDynamic*)serverPlayers.at(source)->GetPhysicsObject()->GetPXActor();
				//body->setLinearDamping(0.4f);

				if (realPacket->buttonstates[0] == 1)
					body->addForce(PhysxConversions::GetVector3(fwdAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[1] == 1)
					body->addForce(PhysxConversions::GetVector3(-rightAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[2] == 1)
					body->addForce(PhysxConversions::GetVector3(-fwdAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[3] == 1)
					body->addForce(PhysxConversions::GetVector3(rightAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[4] == 1 && serverPlayers.at(source)->IsGrounded()) {
					body->addForce(PhysxConversions::GetVector3(Vector3(0, 1, 0)) * force * 500 * dt, PxForceMode::eIMPULSE);
				}

				body->setAngularVelocity(PxVec3(0));
				body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(Maths::DegreesToRadians(realPacket->cameraYaw), { 0, 1, 0 })));
			}
		}
	}
	//CLIENT version of the game will receive these from the servers
	else if (type == Delta_State) {
		//std::cout << "Client: Received DeltaState packet from server." << std::endl;

		DeltaPacket* realPacket = (DeltaPacket*)payload;
		if (realPacket->objectID < (int)networkObjects.size()) {
			networkObjects[realPacket->objectID]->ReadPacket(*realPacket);
		}
	}
	else if (type == Full_State) {
		//std::cout << "Client: Received FullState packet from server." << std::endl;

		FullPacket* realPacket = (FullPacket*)payload;
		if (realPacket->objectID < (int)networkObjects.size()) {
			networkObjects[realPacket->objectID]->ReadPacket(*realPacket);

			thisClient->lastPacketID = realPacket->fullState.stateID;
		}
	}
	else if (type == Message) {
		MessagePacket* realPacket = (MessagePacket*)payload;

		if (realPacket->messageID == COLLISION_MSG) {
			std::cout << "Client: Received collision message!" << std::endl;
		}
	}
	else if (type == Player_Connected) {
		NewPlayerPacket* realPacket = (NewPlayerPacket*)payload;
		std::cout << "Client: New player connected! ID: " << realPacket->playerID << std::endl;

		if (initialising) {
			InitWorld(clientState);

			for (int i = -1; i < realPacket->playerID; i++) {
				SpawnPlayer(i);
			}

			localPlayer = SpawnPlayer(realPacket->playerID);

			if (localPlayerName != "") {
				localPlayer->SetPlayerName(localPlayerName);
			}

			GameManager::SetPlayer(localPlayer);
			GameManager::SetLockedObject(localPlayer);
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);

			/*GameManager::SetSelectionObject(localPlayer);
			localPlayer->SetSelected(true);
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
			GameManager::SetLockedObject(localPlayer);
			GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetWorld()->GetMainCamera()->GetState());*/

			initialising = false;
		}
		else {
			SpawnPlayer(realPacket->playerID);
		}
	}
	else if (type == Player_Disconnected) {
		PlayerDisconnectPacket* realPacket = (PlayerDisconnectPacket*)payload;
		std::cout << "Client: Player Disconnected!" << std::endl;
		GameObject* g = networkObjects[levelNetworkObjectsCount + realPacket->playerID + 1]->GetGameObject();

		if (dynamic_cast<NetworkPlayer*>(g)) {
			NetworkPlayer* player = (NetworkPlayer*)g;
			player->Disconnect();
		}
	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;

		int playerNum = a->GetPlayerNum();

		if (playerNum > -1) {
			newPacket.playerID = b->GetPlayerNum();
			thisServer->SendPacketToPeer(a->GetPlayerNum(), newPacket);
		}

		playerNum = b->GetPlayerNum();

		if (playerNum > -1) {
			newPacket.playerID = a->GetPlayerNum();
			thisServer->SendPacketToPeer(b->GetPlayerNum(), newPacket);
		}
	}
}