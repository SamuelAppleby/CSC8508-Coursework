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
	finishTime = 0.0f;
	finished = false;
}

void PlayerObject::Update(float dt) {
	GameObject::Update(dt);
	raycastTimer -= dt;

	if (!finished) {
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

bool PlayerObject::CheckHasFinished(LevelState state) {
	if (state == LevelState::LEVEL2) {
		if (transform.GetPosition().z < -3600) {
			finished = true;
			finishTime = timeAlive;
		}
	}
	else if (state == LevelState::LEVEL3) {
		if (transform.GetPosition().z < -1800) {
			finished = true;
			finishTime = timeAlive;
		}
	}

	return finished;
}