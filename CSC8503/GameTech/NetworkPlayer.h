#pragma once
#include "../CSC8503Common/GameObject.h"

namespace NCL {
	namespace CSC8503 {
		class NetworkedGame;

		class NetworkPlayer : public GameObject {
		public:
			NetworkPlayer(NetworkedGame* game, int num);
			~NetworkPlayer();

			void OnCollisionBegin(GameObject* otherObject) override;

			int GetPlayerNum() const {
				return playerNum;
			}

			int GetScore() const {
				return score;
			}

			void SetScore(int val) {
				score = val;
			}

			string GetPlayerName() const {
				return playerName;
			}

			void SetPlayerName(string s) {
				playerName = s;
			}

			string GetDefaultPlayerName() const {
				return defaultPlayerName;
			}

		protected:
			NetworkedGame* game;
			int playerNum;
			int score;
			string playerName;
			string defaultPlayerName;
		};
	}
}

