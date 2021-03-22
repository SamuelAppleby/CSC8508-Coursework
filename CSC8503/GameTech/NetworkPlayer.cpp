#include "NetworkPlayer.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int num) {
	this->game = game;
	playerNum = num;
	defaultPlayerName = "Player " + std::to_string(num + 2);
	playerName = defaultPlayerName;
	connected = true;
	isHost = false;
}

NetworkPlayer::~NetworkPlayer() {

}

void NetworkPlayer::Update(float dt) {
	UIState ui = GameManager::GetRenderer()->GetUIState();

	if (isHost && (ui == UIState::INGAME || ui == UIState::SCOREBOARD)) {
		PlayerObject::Update(dt);
	}
	else {
		GameObject::Update(dt);
		raycastTimer -= dt;
	}
}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject) {
	isColliding = true;

	//if (game) {
	/*if (otherObject->GetName() == "Floor") {
		isGrounded = true;
	}
	else if (otherObject->GetName() == "Coin") {
		score += 100;
	}
	else */
	if (dynamic_cast<NetworkPlayer*>(otherObject)) {
		game->OnPlayerCollision(this, (NetworkPlayer*)otherObject);
	}
	//}
}