#include "PlayerObject.h"

using namespace NCL;
using namespace CSC8503;
PlayerObject::PlayerObject() {
	name = "Player";
	isGrounded = false;
	speed = 500000.0f;
	//longJump = 500000.0f;
	raycastTimer = .25f;
	coinsCollected = 0;
	jumpHeight = 6.0f;
}

void PlayerObject::Update(float dt) {
	GameObject::Update(dt);
	raycastTimer -= dt;

	movingForward = (Window::GetKeyboard()->KeyDown(KeyboardKeys::W));
	movingLeft = (Window::GetKeyboard()->KeyDown(KeyboardKeys::A));
	movingBackwards = (Window::GetKeyboard()->KeyDown(KeyboardKeys::S));
	movingRight = (Window::GetKeyboard()->KeyDown(KeyboardKeys::D));
	isJumping = (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isGrounded);

	fwd = Quaternion(transform.GetOrientation()) * Vector3(0, 0, -1);
	right = Vector3::Cross(Vector3(0, 1, 0), -fwd);
}

void PlayerObject::FixedUpdate(float fixedDT) {

	//std::cout << fixedDT;
	PxVec3 playerVel = ((PxRigidDynamic*)physicsObject->GetPXActor())->getLinearVelocity();
	if (movingForward)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(fwd) * speed * fixedDT, PxForceMode::eIMPULSE);
	if (movingLeft)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(-right) * speed * fixedDT, PxForceMode::eIMPULSE);
	if (movingBackwards)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(-fwd) * speed * fixedDT, PxForceMode::eIMPULSE);
	if (movingRight)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(right) * speed * fixedDT, PxForceMode::eIMPULSE);
	if (isJumping)
		playerVel.y = sqrt(jumpHeight * -2 * NCL::CSC8503::GRAVITTY);
	((PxRigidDynamic*)physicsObject->GetPXActor())->setLinearVelocity(playerVel);
}
