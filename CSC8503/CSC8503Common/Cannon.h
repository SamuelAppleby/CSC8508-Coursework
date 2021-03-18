#pragma once
#include "GameObject.h"
#include "Cannonball.h"
#include "../GameTech/GameManager.h"
namespace NCL
{
	namespace CSC8503
	{
		class Cannon : public GameObject
		{
		public:
			Cannon(PxVec3 newTrajectory, int newShotTimes = 10, int newShotSize = 5)
			{
				shotSize = newShotSize;
				shotTimes = newShotTimes;
				trajectory = newTrajectory;
				timeSinceShot = 0;
				currentBall = nullptr;
			}

			~Cannon()
			{
				SetRenderObject(NULL);
				SetPhysicsObject(NULL);
			}

			void Update(float dt) override;

			void Shoot();
			

			float getTimeSinceShot()
			{
				return timeSinceShot;
			}

			//PxVec3 shoot()
			//{
			//timeSinceShot = 0;
			//return trajectory;
			//}

		

			

			int getMaxAlive()
			{
				return shotTimes;
			}

			int getShotSize()
			{
				return shotSize;
			}

			int getShotDensity()
			{
				return shotDensity;
			}
		protected:
			int shotDensity;
			int shotSize;
			int shotTimes;
			float timeSinceShot;
			PxVec3 trajectory;
			Cannonball* currentBall;

		};
	}
}


