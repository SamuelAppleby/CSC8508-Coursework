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
	isActive = true;
	physicsObject = nullptr;
	renderObject = nullptr;
	selectedObject = false;
	timeInSet = 0.0f;
	powerUpTimer = 0.0f;
	timeAlive = 0.0f;
	safeForDeletion = false;
}

GameObject::~GameObject() {
	delete physicsObject;
	delete renderObject;
}


