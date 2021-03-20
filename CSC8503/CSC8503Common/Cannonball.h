#pragma once
#include "GameObject.h"
#include "GameWorld.h"

namespace NCL
{
	namespace CSC8503
	{
		class Cannonball :
			public GameObject
		{
		public:
			Cannonball() {
				name = "Cannonball";
				isActive = false;
				timeLeft = 10.f;
				//Disable();
			}

			bool IsActive() const {
				return isActive;
			}

			void Update(float dt) override;
			void ResetBall(const PxVec3& newPos, const PxVec3& force);

		protected:
			float timeLeft;
			void Disable();
			PxVec3 initialPos;
			bool isActive;
		};
	}
}
