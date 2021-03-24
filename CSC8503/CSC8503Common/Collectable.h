#pragma once
#include "GameObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class Collectable : public GameObject
		{
		public:
			Collectable() {
				name = "Collectable";
			}
			virtual void OnCollisionBegin(GameObject* otherObject) {
				if (otherObject->GetName() == "Player") {
					SetPosition(PxVec3(5000, 5000, 5000));
					canDestroy = true;
				}
			}
		};
	}
}


