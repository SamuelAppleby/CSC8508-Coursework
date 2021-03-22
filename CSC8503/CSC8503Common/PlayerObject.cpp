#include "PlayerObject.h"
#include "../GameTech/GameManager.h"

using namespace NCL;
using namespace CSC8503;
PlayerObject::PlayerObject() : GameObject("Player") {
	//name = "Player";
	isGrounded = false;
	speed = 500000.0f;
	raycastTimer = .25f;
	coinsCollected = 0;
}

void PlayerObject::Update(float dt) {
	GameObject::Update(dt);
	raycastTimer -= dt;

	UIState ui = GameManager::GetRenderer()->GetUIState();

	if (ui == UIState::INGAME || ui == UIState::SCOREBOARD) {
		Vector3 fwd = Quaternion(transform.GetOrientation()) * Vector3(0, 0, -1);
		Vector3 right = Vector3::Cross(Vector3(0, 1, 0), -fwd);

		PxRigidDynamic* body = (PxRigidDynamic*)physicsObject->GetPXActor();

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
			body->addForce(PhysxConversions::GetVector3(fwd) * speed * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
			body->addForce(PhysxConversions::GetVector3(-right) * speed * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
			body->addForce(PhysxConversions::GetVector3(-fwd) * speed * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
			body->addForce(PhysxConversions::GetVector3(right) * speed * dt, PxForceMode::eIMPULSE);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isGrounded)
			body->addForce(PhysxConversions::GetVector3(Vector3(0, 1, 0)) * speed * 500 * dt, PxForceMode::eIMPULSE);
	}
}