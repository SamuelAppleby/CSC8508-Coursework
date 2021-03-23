#pragma once
//#include <thread>
//#include <atomic>

#include "NetworkBase.h"
#include "../GameTech/GameManager.h"

namespace NCL {
	namespace CSC8503 {
		class GameWorld;
		class NetworkPlayer;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients, LevelState level);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld& g);

			void ThreadedUpdate();

			bool SendGlobalPacket(int msgID);
			bool SendGlobalPacket(GamePacket& packet);
			bool SendPacketToPeer(int peerID, GamePacket& packet);

			virtual void UpdateServer(float dt);

			std::map<int, ENetPeer*> players;

		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			GameWorld* gameWorld;
			LevelState level;

			/*std::atomic<bool> threadAlive;
			std::thread updateThread;*/

			int incomingDataRate;
			int outgoingDataRate;
		};
	}
}
