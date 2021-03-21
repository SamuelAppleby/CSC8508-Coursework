#include "PlayerObject.h"

using namespace NCL;
using namespace CSC8503;
PlayerObject::PlayerObject() {
	name = "Player";
	isGrounded = false;
	speed = 500000.0f;
	raycastTimer = .25f;
	coinsCollected = 0;
}

void PlayerObject::Update(float dt) {
	GameObject::Update(dt);
	raycastTimer -= dt;

	Vector3 fwd = Quaternion(transform.GetOrientation()) * Vector3(0, 0, -1);
	Vector3 right = Vector3::Cross(Vector3(0, 1, 0), -fwd);

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(fwd) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(-right) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(-fwd) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(right) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isGrounded) 
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(Vector3(0, 1, 0)) * speed * 500 * dt, PxForceMode::eIMPULSE);

	((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PxVec3(0, -1.1, 0));
}
