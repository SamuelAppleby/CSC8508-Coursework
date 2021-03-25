#pragma once

#include "Collectable.h"
#include "PlayerObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class LongJump : public Collectable
		{
		public:
			LongJump() {
				name = "LongJump";
			}
			void OnCollisionBegin(GameObject* otherObject) override {
				Collectable::OnCollisionBegin(otherObject);
				if (otherObject->GetName() == "Player") {
					((PlayerObject*)otherObject)->LongJumpCollection();
				}
			}
		};
	}
}
