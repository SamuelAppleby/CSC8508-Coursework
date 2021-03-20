#pragma once
#include "LevelCreator.h"
#include "../CSC8503Common/NetworkObject.h"
#include "../CSC8503Common/NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public LevelCreator, public PacketReceiver {
		public:
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer(LevelState state);
			void StartAsClient(LevelState state, string ip);

			void Update(float dt) override;

			NetworkPlayer* SpawnPlayer(int playerNum);
			void AddLevelNetworkObject(GameObject* object);

			void InitWorld(LevelState state);
			void InitGameObstacles(LevelState state);

			void ReceivePacket(float dt, int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot(bool deltaFrame);
			void UpdateMinimumState();

			// Server-side
			std::map<int, int> stateIDs;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;

			// Client-side
			std::vector<NetworkObject*> networkObjects;
			LevelState clientState;

			// Server-side
			std::map<int, NetworkPlayer*> serverPlayers;

			NetworkPlayer* localPlayer = nullptr;
			string localPlayerName = "";

			int levelNetworkObjectsCount = 0;
			bool initialising = true;
		};
	}
}

