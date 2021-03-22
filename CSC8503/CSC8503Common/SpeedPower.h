#pragma once

#include "Collectable.h"
#include "PlayerObject.h"
namespace NCL
{
	namespace CSC8503
	{
		class SpeedPower : public Collectable
		{
		public:
			SpeedPower() {
				name = "SpeedPower";
			}
			void OnCollisionBegin(GameObject* otherObject) override {
				Collectable::OnCollisionBegin(otherObject);
				if (otherObject->GetName() == "Player") {
					((PlayerObject*)otherObject)->SpeedPowerColelction();
				}
			}
		};
	}
}
