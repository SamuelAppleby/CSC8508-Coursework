#include "PlayerObject.h"
#include "../../Common/Win32Window.h"
#include "../CSC8503Common/PhysxConversions.h"
#include "../GameTech/GameManager.h"

using namespace NCL;
using namespace CSC8503;
PlayerObject::PlayerObject() {
	name = "Player";
	isGrounded = false;
	speed = 500000.0f;
	raycastTimer = .25f;
}

void PlayerObject::Update(float dt) {
	GameObject::Update(dt);
	raycastTimer -= dt;
	if(raycastTimer <= .0f)
		isGrounded = CheckGrounded();

	Matrix4 view = GameManager::GetWorld()->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld = view.Inverse();
	Vector3 rightAxis = Vector3(camWorld.GetColumn(0));

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(fwdAxis) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(-rightAxis) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(-fwdAxis) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D))
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(rightAxis) * speed * dt, PxForceMode::eIMPULSE);
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE) && isGrounded) 
		((PxRigidDynamic*)physicsObject->GetPXActor())->addForce(PhysxConversions::GetVector3(Vector3(0, 1, 0)) * speed * 500 * dt, PxForceMode::eIMPULSE);
}

bool PlayerObject::CheckGrounded() {
	raycastTimer = .25f;
	PxVec3 pos = PhysxConversions::GetVector3(transform.GetPosition()) + PxVec3(0, -6, 0 );
	PxVec3 dir = PxVec3(0, -1, 0);
	float distance = 4.0f;
	PxRaycastBuffer hit;

	if (GameManager::GetPhysicsSystem()->GetGScene()->raycast(pos, dir, distance, hit)) {
		GameObject* obj = GameManager::GetWorld()->FindObjectFromPhysicsBody(hit.block.actor);
		if(obj->GetName() == "Floor")
			return true;
	}
	return false;
}
