/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game Object Implementation		 */
#include "GameObject.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName) {
	name = objectName;
	worldID = -1;
	physicsObject = nullptr;
	renderObject = nullptr;
	selectedObject = false;
	timeAlive = 0.0f;
	timeInSet = 0.0f;
	powerUpTimer = 0.0f;
	isGrounded = false;
}

GameObject::~GameObject() {
	delete physicsObject;
	delete renderObject;
}

void GameObject::OnCollisionBegin(GameObject* otherObject) {
	std::cout << name << " collided with: " << otherObject->GetName() << std::endl;

	if (otherObject->GetName() == "Floor") {
		isGrounded = true;
	}
}

void GameObject::OnCollisionEnd(GameObject* otherObject) {
	if (otherObject->GetName() == "Floor") {
		isGrounded = false;
	}
}


