#include "PlayerObject.h"
#include "../GameTech/GameManager.h"

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
	jumpHeight = 15.0f;
	state = PowerUpState::NORMAL;
	finishTime = 0.0f;
	finished = false;
}


void PlayerObject::Update(float dt)
{

	Vector3 q = Quaternion(physicsObject->GetPXActor()->getGlobalPose().q).ToEuler() + Vector3(0, 180, 0);
	transform.SetOrientation(PhysxConversions::GetQuaternion(Quaternion::EulerAnglesToQuaternion(q.x, q.y, q.z)));
	transform.SetPosition(physicsObject->GetPXActor()->getGlobalPose().p);
	transform.UpdateMatrix();
	timeAlive += dt;

	if (powerUpTimer > 0.0f)
	{
		powerUpTimer -= dt;
	}
	else
	{
		switch (state)
		{
		case PowerUpState::LONGJUMP:
			jumpHeight = 15.0f;
			break;
		}
		state = PowerUpState::NORMAL;
	}
	raycastTimer -= dt;

	movingForward = (Window::GetKeyboard()->KeyDown(KeyboardKeys::W));
	movingLeft = (Window::GetKeyboard()->KeyDown(KeyboardKeys::A));
	movingBackwards = (Window::GetKeyboard()->KeyDown(KeyboardKeys::S));
	movingRight = (Window::GetKeyboard()->KeyDown(KeyboardKeys::D));
	isSprinting = (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT));
	isJumping = (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isGrounded);

	fwd = PhysxConversions::GetVector3(Quaternion(transform.GetOrientation()) * Vector3(0, 0, 1));
	right = PhysxConversions::GetVector3(Vector3::Cross(Vector3(0, 1, 0), -fwd));
}

void PlayerObject::FixedUpdate(float fixedDT) {
	if (!finished)
	{
		speed = isGrounded ? 2000.0f : 1000.0f;	// air damping
		PxRigidDynamic* body = (PxRigidDynamic*)physicsObject->GetPXActor();

		MAX_SPEED = isSprinting ? (state == PowerUpState::SPEEDPOWER ? 160.0f : 80.0f) : (state == PowerUpState::SPEEDPOWER ? 100.0f : 50.0f);
		if (movingForward)
			body->addForce(fwd * speed, PxForceMode::eIMPULSE);
		if (movingLeft)
			body->addForce(-right * speed, PxForceMode::eIMPULSE);
		if (movingBackwards)
			body->addForce(-fwd * speed, PxForceMode::eIMPULSE);
		if (movingRight)
			body->addForce(right * speed, PxForceMode::eIMPULSE);

		PxVec3 playerVel = body->getLinearVelocity();
		if (isJumping)
			playerVel.y = sqrt(jumpHeight * -2 * NCL::CSC8503::GRAVITTY);

		float linearSpeed = PxVec3(playerVel.x, 0, playerVel.z).magnitude();
		float excessSpeed = std::clamp(linearSpeed - MAX_SPEED, 0.0f, 0.1f);
		if (excessSpeed) {
			body->addForce(-playerVel * excessSpeed, PxForceMode::eVELOCITY_CHANGE);
		}

		body->setLinearVelocity(playerVel);
	}
}

bool PlayerObject::CheckHasFinished(LevelState state)
{
	Vector3 pos = transform.GetPosition();

	if (state == LevelState::LEVEL2)
	{
		if (pos.z < -3600 && pos.y > -177 && pos.x > -20 && pos.x < 20)
		{
			finished = true;
			finishTime = timeAlive;
		}
	}
	else if (state == LevelState::LEVEL3)
	{
		if (pos.z < -1950 && pos.y > 182 && pos.x > -101 && pos.x < 101)
		{
			finished = true;
			finishTime = timeAlive;
		}
	}

	return finished;

}

