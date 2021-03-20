#pragma once
#include "GameObject.h"

namespace NCL
{
	namespace CSC8503
	{
		class Cannonball :
			public GameObject
		{
		public:
			Cannonball()
			{
				name = "Cannonball";
				isActive = false;
				timeLeft = 10.f;
				//Disable();
			}


			void Update(float dt) override;
			void ResetBall(const PxVec3& newPos, const PxVec3& force);


			void setDestroy(bool newDestroy)
			{
				destroy = newDestroy;
			}

			bool getDestroy()
			{
				return destroy;
			}

			bool isActive;

		protected:

			float timeLeft;
			void Disable();
			PxVec3 initialPos;
		};
	}
}
