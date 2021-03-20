#include "NetworkPlayer.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int num) : GameObject("Player") {
	this->game = game;
	playerNum = num;
	score = 0;
	defaultPlayerName = "Player " + num;
	playerName = defaultPlayerName;
}

NetworkPlayer::~NetworkPlayer() {

}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject) {
	isColliding = true;

	//if (game) {
	if (otherObject->GetName() == "Floor") {
		isGrounded = true;
	}
	else if (otherObject->GetName() == "Coin") {
		score += 100;
	}
	else if (dynamic_cast<NetworkPlayer*>(otherObject)) {
		game->OnPlayerCollision(this, (NetworkPlayer*)otherObject);
	}
	//}
}