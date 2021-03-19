/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game Object Implementation		 */
#include "GameObject.h"

using namespace NCL::CSC8503;




GameObject::GameObject(string objectName)
{
	name = objectName;
	worldID = -1;
	physicsObject = nullptr;
	renderObject = nullptr;
	selectedObject = false;
	timeAlive = 0.0f;
	timeInSet = 0.0f;
	powerUpTimer = 0.0f;
	isGrounded = false;
	isColliding = false;
	//transform = 
}

GameObject::~GameObject()
{
	delete physicsObject;
	delete renderObject;
}

void GameObject::Update(float dt)
{

	transform.SetOrientation(physicsObject->GetPXActor()->getGlobalPose().q);
	transform.SetPosition(physicsObject->GetPXActor()->getGlobalPose().p);
	transform.UpdateMatrix();
	timeAlive += dt;

}



void GameObject::OnCollisionBegin(GameObject* otherObject)
{
	//std::cout << name << " collided with: " << otherObject->GetName() << std::endl;
	isColliding = true;
	if (otherObject->GetName() == "Floor")
	{
		isGrounded = true;
	}
}

void GameObject::OnCollisionEnd(GameObject* otherObject)
{
	//std::cout << name << " stopped collision with: " << otherObject->GetName() << std::endl;
	isColliding = false;
	if (otherObject->GetName() == "Floor")
	{
		isGrounded = false;
	}
}


void GameObject::SetPosition(const PxVec3& worldPos)
{

	PxRigidDynamic* actor = (PxRigidDynamic*)physicsObject->GetPXActor();
	actor->setGlobalPose(PxTransform(worldPos));
	transform.UpdateMatrix();
}




void GameObject::SetOrientation(const PxQuat& worldOrientation)
{
	PxRigidDynamic* actor = (PxRigidDynamic*)physicsObject->GetPXActor();
	actor->setGlobalPose(PxTransform(transform.GetPosition(), worldOrientation));
	transform.UpdateMatrix();
}

//void GameObject::SetScale(const PxVec3& newScale)
//{
//	PxRigidDynamic* actor = (PxRigidDynamic*)physicsObject->GetPXActor();
//	//actor->getShapes()
//}


