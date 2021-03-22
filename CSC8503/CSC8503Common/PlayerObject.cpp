#include "PlayerObject.h"

using namespace NCL;
using namespace CSC8503;
PlayerObject::PlayerObject()
{
	name = "Player";
	movingForward = false;
	movingBackwards = false;
	movingLeft = false;
	movingRight = false;
	isJumping = false;
	isGrounded = false;
	score = 0;
	speed = 2000.0f;
	raycastTimer = .25f;
	powerUpTimer = 0.0f;
	coinsCollected = 0;
	jumpHeight = 10.0f;
}


void PlayerObject::Update(float dt)
{

	Vector3 q = Quaternion(physicsObject->GetPXActor()->getGlobalPose().q).ToEuler() + Vector3(0, 180, 0);
	transform.SetOrientation(PhysxConversions::GetQuaternion(Quaternion::EulerAnglesToQuaternion(q.x, q.y, q.z)));
	transform.SetPosition(physicsObject->GetPXActor()->getGlobalPose().p);
	transform.UpdateMatrix();
	timeAlive += dt;

	/*if (powerUpTimer > 0.0f)
	{
		jumpHeight = 40.0f;
		powerUpTimer -= dt;
	}
	else
	{
		jumpHeight = 20.0f;
	}*/

	if (powerUpTimer > 0.0f)
	{
		switch (state)
		{
		case PowerUpState::LONGJUMP:
			jumpHeight = 20.0f;
			powerUpTimer -= dt;
			break;
		case PowerUpState::SPEEDPOWER:
			speed = speed * 10;
			powerUpTimer -= dt;
			break;

		}
	}
	else
	{
		speed = 2000.0f;
		jumpHeight = 10.0f;
	}
	raycastTimer -= dt;

	movingForward = (Window::GetKeyboard()->KeyDown(KeyboardKeys::W));
	movingLeft = (Window::GetKeyboard()->KeyDown(KeyboardKeys::A));
	movingBackwards = (Window::GetKeyboard()->KeyDown(KeyboardKeys::S));
	movingRight = (Window::GetKeyboard()->KeyDown(KeyboardKeys::D));
	isSprinting = (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT) && isGrounded);
	isJumping = (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isGrounded);

	fwd = PhysxConversions::GetVector3(Quaternion(transform.GetOrientation()) * Vector3(0, 0, 1));
	right = PhysxConversions::GetVector3(Vector3::Cross(Vector3(0, 1, 0), -fwd));
}

void PlayerObject::FixedUpdate(float fixedDT) {
	speed = isGrounded ? 2000.0f : 1000.0f;	// air damping
	MAX_SPEED = isSprinting ? 80.0f : 50.0f;
	if (movingForward)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(fwd * speed, PxForceMode::eIMPULSE);
	if (movingLeft)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(-right * speed, PxForceMode::eIMPULSE);
	if (movingBackwards)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(-fwd * speed, PxForceMode::eIMPULSE);
	if (movingRight)
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(right * speed, PxForceMode::eIMPULSE);
	
	PxVec3 playerVel = ((PxRigidDynamic*)physicsObject->GetPXActor())->getLinearVelocity();
	if (isJumping)
		playerVel.y = sqrt(jumpHeight * -2 * NCL::CSC8503::GRAVITTY);
	
	float linearSpeed = PxVec3(playerVel.x, 0, playerVel.z).magnitude();
	float excessSpeed = std::clamp(linearSpeed - MAX_SPEED, 0.0f, 0.1f);
	if (excessSpeed) {
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(-playerVel * excessSpeed, PxForceMode::eVELOCITY_CHANGE);
	}
	float ySpeed = playerVel.y;
	float excessYSpeed = std::clamp(ySpeed - 120, 0.0f, 0.1f);
	if (excessYSpeed) {
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(-PxVec3(0, ySpeed, 0) * excessYSpeed, PxForceMode::eVELOCITY_CHANGE);
	}
	((PxRigidDynamic*)physicsObject->GetPXActor())->setLinearVelocity(playerVel);
}

