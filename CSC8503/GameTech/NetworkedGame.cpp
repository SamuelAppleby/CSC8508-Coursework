#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "../CSC8503Common/GameServer.h"
#include "../CSC8503Common/GameClient.h"

#define COLLISION_MSG 30

struct MessagePacket : public GamePacket
{
	short playerID;
	short messageID;

	MessagePacket()
	{
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame()
{
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket = 0.0f;
	packetsToSnapshot = 0;
}

NetworkedGame::~NetworkedGame()
{
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer(LevelState state)
{
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Received_State, this);

	//std::cout << "Starting as server." << std::endl;

	InitWorld(state);
	localPlayer = SpawnPlayer(-1);

	GameManager::SetSelectionObject(localPlayer);
	localPlayer->SetSelected(true);
	GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
	GameManager::SetLockedObject(localPlayer);
	GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetWorld()->GetMainCamera()->GetState());
}

void NetworkedGame::StartAsClient(LevelState state, string ip)
{
	thisClient = new GameClient();
	thisClient->Connect(ip, NetworkBase::GetDefaultPort());

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);

	//std::cout << "Starting as client." << std::endl;

	// Set localPlayerName here from UI input

	clientState = state;
}

void NetworkedGame::Update(float dt)
{
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0)
	{
		if (thisServer)
		{
			UpdateAsServer(dt);
		}
		else if (thisClient)
		{
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / 120.0f; //120hz server/client update
	}

	if (thisServer)
	{
		FixedUpdate(dt);
	}


	UpdateLevel(dt);
	GameManager::GetWorld()->UpdateWorld(dt);
	GameManager::GetAudioManager()->UpdateAudio(dt);
	GameManager::GetRenderer()->Update(dt);
	GameManager::GetRenderer()->Render();
	Debug::FlushRenderables(dt);
}

void NetworkedGame::UpdateAsServer(float dt)
{
	thisServer->UpdateServer(dt);

	if (serverPlayers.size() < thisServer->players.size())
	{
		for (auto i : thisServer->players)
		{
			std::map<int, NetworkPlayer*>::iterator it;
			it = serverPlayers.find(i.first);

			if (it == serverPlayers.end())
			{
				serverPlayers.insert(std::pair<int, NetworkPlayer*>(i.first, SpawnPlayer(i.first)));
				stateIDs.insert(std::pair<int, int>(i.first, 0));
			}
		}
	}
	else if (serverPlayers.size() < thisServer->players.size())
	{
		for (auto i : serverPlayers)
		{
			std::map<int, ENetPeer*>::iterator it;
			it = thisServer->players.find(i.first);

			if (it == thisServer->players.end())
			{
				serverPlayers.erase(i.first);
				stateIDs.erase(i.first);
			}
		}
	}

	if (stateIDs.size() > 0)
	{
		UpdateMinimumState();
	}

	packetsToSnapshot--;
	if (packetsToSnapshot < 0)
	{
		BroadcastSnapshot(false);
		packetsToSnapshot = 5;
	}
	else
	{
		BroadcastSnapshot(true);
	}
}

void NetworkedGame::UpdateAsClient(float dt)
{
	ClientPacket newPacket;
	thisClient->UpdateClient(dt);

	Camera* c = GameManager::GetWorld()->GetMainCamera();
	Matrix4 view = c->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	newPacket.rightAxis = Vector3(camWorld.GetColumn(0));
	newPacket.cameraYaw = c->GetYaw();

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
		newPacket.buttonstates[0] = 1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
		newPacket.buttonstates[1] = 1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
		newPacket.buttonstates[2] = 1;
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
		newPacket.buttonstates[3] = 1;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE))
	{
		newPacket.buttonstates[4] = 1;
	}
	newPacket.lastID = thisClient->lastPacketID;
	thisClient->SendPacket(newPacket);
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame)
{
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	GameManager::GetWorld()->GetObjectIterators(first, last);

	for (auto p : serverPlayers)
	{
		for (auto i = first; i != last; ++i)
		{ // Change to loop through networkObjects?
			NetworkObject* o = (*i)->GetNetworkObject();
			if (!o)
			{
				continue;
			}

			int playerState = stateIDs.at(p.first);
			GamePacket* newPacket = nullptr;
			if (o->WritePacket(&newPacket, deltaFrame, playerState))
			{
				thisServer->SendPacketToPeer(p.first, *newPacket);
				delete newPacket;
			}
		}
	}
}

void NetworkedGame::UpdateMinimumState()
{
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs)
	{
		minID = min(minID, i.second);
		maxID = max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	GameManager::GetWorld()->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i)
	{
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o)
		{
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}

	//std::cout << "Server min state: " << minID << std::endl;
}

NetworkPlayer* NetworkedGame::SpawnPlayer(int playerNum)
{
	NetworkPlayer* p = GameManager::AddPxNetworkPlayerToWorld(PxTransform(PxVec3(-5 * playerNum, 1, 0)), 1, this, playerNum);
	NetworkObject* n = new NetworkObject(*p, levelNetworkObjectsCount + playerNum + 1);

	return p;
}

void NetworkedGame::InitWorld(LevelState state)
{
	InitCamera();
	GameManager::SetLevelState(state);
	InitFloors(state);
	InitGameObstacles(state);
}

//void NetworkedGame::InitGameObstacles(LevelState state) {
//	NetworkObject* n;
//
//	switch (state)
//	{
//	case LevelState::LEVEL1:
//		GameManager::AddPxSphereToWorld(PxTransform(PxVec3(-20, 20, -20)), 2);
//		GameManager::AddPxCubeToWorld(PxTransform(PxVec3(0, 20, -20)), PxVec3(2, 2, 2));
//		GameManager::AddPxCapsuleToWorld(PxTransform(PxVec3(20, 20, -20)), 2, 2);
//		break;
//	case LevelState::LEVEL2:
//		//HAVE COMMENTED OUT THE ORIGINAL BEAMS, WILL LEAVE IN IN CASE WE DECIDE TO GO FOR STATIC ONES
//		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 200));
//		n = new NetworkObject(*GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(-70, -98, -900)), PxVec3(20, 20, 198), PxVec3(0, 0, 1)), networkObjects.size());
//		networkObjects.emplace_back(n);
//		levelNetworkObjectsCount++;
//
//		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(0, -98, -900)), PxVec3(20, 20, 200));
//		n = new NetworkObject(*GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(0, -98, -900)), PxVec3(20, 20, 198), PxVec3(0, 0, 2)), networkObjects.size());
//		networkObjects.emplace_back(n);
//		levelNetworkObjectsCount++;
//
//		//WorldCreator::AddPxFloorToWorld(PxTransform(PxVec3(70, -98, -900)), PxVec3(20, 20, 200));
//		n = new NetworkObject(*GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(70, -98, -900)), PxVec3(20, 20, 198), PxVec3(0, 0, 1)), networkObjects.size());
//		networkObjects.emplace_back(n);
//		levelNetworkObjectsCount++;
//
//		//cannons
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -850)), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -900)), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-150, -70, -950)), PxVec3(700000000, 8500, 0), 10, 10, PxVec3(35, 0, 0));
//
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -825)), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -875)), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -925)), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(150, -70, -975)), PxVec3(-700000000, 8500, 0), 10, 10, PxVec3(-35, 0, 0));
//
//		//OBSTACLE 5 - THE BLENDER
//		//basically, it's an enclosed space with a spinning arm at the bottom to randomise which player actually wins
//		//it should be flush with the entrance to the podium room so that the door is reasonably difficult to access unless there's nobody else there
//		//again, not sure how to create the arm, it's a moving object, might need another class for this
//		//also, it's over a 100m drop to the blender floor, so pls don't put fall damage in blender blade
//		n = new NetworkObject(*GameManager::AddPxRotatingCubeToWorld(PxTransform(PxVec3(0, -78, -1705)), PxVec3(190, 20, 20), PxVec3(0, 1, 0)), networkObjects.size());
//		networkObjects.emplace_back(n);
//		levelNetworkObjectsCount++;
//
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-80, 100, -1351)), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(-40, 100, -1351)), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(0, 100, -1351)), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(40, 100, -1351)), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
//		GameManager::AddPxCannonToWorld(PxTransform(PxVec3(80, 100, -1351)), PxVec3(1, 1, 700000), 20, 20, PxVec3(0, 0, 25));
//
//
//		for (int i = 0; i < 30; i++)
//		{
//			Cannonball* c = GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 20);
//			n = new NetworkObject(*c, networkObjects.size());
//			networkObjects.emplace_back(n);
//			levelNetworkObjectsCount++;
//			GameManager::GetObstacles()->cannons.push_back(c);
//		}
//
//		/*
//		for (int i = 0; i < 5; i++)
//		{
//			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 20));
//		}
//
//		for (int i = 0; i < 7; i++)
//		{
//			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 10));
//		}
//
//		for (int i = 0; i < 5; i++)
//		{
//			GameManager::GetObstacles()->cannons.push_back(GameManager::AddPxCannonBallToWorld(PxTransform(PxVec3(50000, 5000, 5000)), 20));
//		}
//		*/
//		//PxTransform t = PxTransform(PxVec3(0, 50, 0));
//
//		//GameManager::AddPxCannonBallToWorld(t);
//
//		break;
//	}
//}

void NetworkedGame::ReceivePacket(float dt, int type, GamePacket* payload, int source)
{
	//std::cout << "SOME SORT OF PACKET RECEIVED" << std::endl;
	if (type == Received_State)
	{	//Server version of the game receives these from players
//std::cout << "Server: Received packet from client " << source << "." << std::endl;
		ClientPacket* realPacket = (ClientPacket*)payload;

		std::map<int, int>::iterator it;
		it = stateIDs.find(source);

		if (it != stateIDs.end())
		{
			it->second = realPacket->lastID;
		}

		if (!serverPlayers.empty())
		{
			Vector3 rightAxis = realPacket->rightAxis;

			Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
			fwdAxis.y = 0.0f;
			fwdAxis.Normalise();
			float force = 1000000.0f;

			NetworkPlayer* player = serverPlayers.at(source);

			player->SetPlayerName(realPacket->playerName);

			if (player->GetPhysicsObject()->GetPXActor()->is<PxRigidDynamic>())
			{
				PxRigidDynamic* body = (PxRigidDynamic*)serverPlayers.at(source)->GetPhysicsObject()->GetPXActor();
				body->setLinearDamping(0.4f);

				if (realPacket->buttonstates[0] == 1)
					body->addForce(PhysxConversions::GetVector3(fwdAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[1] == 1)
					body->addForce(PhysxConversions::GetVector3(-rightAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[2] == 1)
					body->addForce(PhysxConversions::GetVector3(-fwdAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[3] == 1)
					body->addForce(PhysxConversions::GetVector3(rightAxis) * force * dt, PxForceMode::eIMPULSE);
				if (realPacket->buttonstates[4] == 1 && serverPlayers.at(source)->IsGrounded())
				{
					body->addForce(PhysxConversions::GetVector3(Vector3(0, 1, 0)) * force * 500 * dt, PxForceMode::eIMPULSE);
				}

				body->setAngularVelocity(PxVec3(0));
				body->setGlobalPose(PxTransform(body->getGlobalPose().p, PxQuat(Maths::DegreesToRadians(realPacket->cameraYaw), { 0, 1, 0 })));
			}
		}
	}
	//CLIENT version of the game will receive these from the servers
	else if (type == Delta_State)
	{
		//std::cout << "Client: Received DeltaState packet from server." << std::endl;

		DeltaPacket* realPacket = (DeltaPacket*)payload;
		if (realPacket->objectID < (int)networkObjects.size())
		{
			networkObjects[realPacket->objectID]->ReadPacket(*realPacket);
		}
	}
	else if (type == Full_State)
	{
		//std::cout << "Client: Received FullState packet from server." << std::endl;

		FullPacket* realPacket = (FullPacket*)payload;
		if (realPacket->objectID < (int)networkObjects.size())
		{
			networkObjects[realPacket->objectID]->ReadPacket(*realPacket);

			thisClient->lastPacketID = realPacket->fullState.stateID;
		}
	}
	else if (type == Message)
	{
		MessagePacket* realPacket = (MessagePacket*)payload;

		if (realPacket->messageID == COLLISION_MSG)
		{
			std::cout << "Client: Received collision message!" << std::endl;
		}
	}
	else if (type == Player_Connected)
	{
		NewPlayerPacket* realPacket = (NewPlayerPacket*)payload;
		std::cout << "Client: New player connected! ID: " << realPacket->playerID << std::endl;

		if (initialising)
		{
			InitWorld(clientState);

			for (int i = -1; i < realPacket->playerID; i++)
			{
				networkObjects.emplace_back(SpawnPlayer(i)->GetNetworkObject());
			}

			localPlayer = SpawnPlayer(realPacket->playerID);
			networkObjects.emplace_back(localPlayer->GetNetworkObject());

			if (localPlayerName != "")
			{
				localPlayer->SetPlayerName(localPlayerName);
			}

			GameManager::SetSelectionObject(localPlayer);
			localPlayer->SetSelected(true);
			GameManager::GetWorld()->GetMainCamera()->SetState(CameraState::THIRDPERSON);
			GameManager::SetLockedObject(localPlayer);
			GameManager::GetWorld()->GetMainCamera()->SetState(GameManager::GetWorld()->GetMainCamera()->GetState());

			initialising = false;
		}
		else
		{
			networkObjects.emplace_back(SpawnPlayer(realPacket->playerID)->GetNetworkObject());
		}
	}
	else if (type == Player_Disconnected)
	{
		PlayerDisconnectPacket* realPacket = (PlayerDisconnectPacket*)payload;
		std::cout << "Client: Player Disconnected!" << std::endl;
	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b)
{
	if (thisServer)
	{ //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;

		int playerNum = a->GetPlayerNum();

		if (playerNum > -1)
		{
			newPacket.playerID = b->GetPlayerNum();
			thisServer->SendPacketToPeer(a->GetPlayerNum(), newPacket);
		}

		playerNum = b->GetPlayerNum();

		if (playerNum > -1)
		{
			newPacket.playerID = a->GetPlayerNum();
			thisServer->SendPacketToPeer(b->GetPlayerNum(), newPacket);
		}
	}
}