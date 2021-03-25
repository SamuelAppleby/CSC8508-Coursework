#pragma once
#include "GameObject.h"
#include "../CSC8503Common/PhysxConversions.h"
#include "../CSC8503Common/PxPhysicsSystem.h"

enum class PowerUpState { NORMAL, LONGJUMP, SPEEDPOWER };

enum class LevelState;

namespace NCL
{
	namespace CSC8503
	{
		class PlayerObject : public GameObject
		{
			float MAX_SPEED = 50.0f;
		public:
			PlayerObject();

			void Update(float dt) override;
			void FixedUpdate(float fixedDT) override;

			//void PowerUpCheck(PowerUpState state);

			bool CheckHasFinished(LevelState state);

			void SetIsGrounded(bool val)
			{
				isGrounded = val;
			}

			void SetRaycastTimer(float val)
			{
				raycastTimer = val;
			}

			float GetRaycastTimer() const
			{
				return raycastTimer;
			}

			void CollectCoin()
			{
				coinsCollected++;
			}

			int GetCoinsCollected() const
			{
				return coinsCollected;
			}
			void SetFinished(bool val)
			{
				finished = val;
			}
			bool HasFinished() const
			{
				return finished;
			}
			void SetFinishTime(float time)
			{
				finishTime = time;
			}
			float GetFinishTime() const
			{
				return finishTime;
			}

			bool IsGrounded() const
			{ 
				return isGrounded;
			}

			float GetJumpHeight() const
			{
				return jumpHeight;
			}

			PowerUpState GetState() const
			{
				return state;
			}

			float GetPowerUpTimer() const
			{
				return powerUpTimer;
			}

			PowerUpState GetPowerUpState()
			{
				return state;
			}

			void LongJumpCollection()
			{
				powerUpTimer = 5.0f;
				jumpHeight = 30.0f;
				state = PowerUpState::LONGJUMP;
			}

			void SpeedPowerCollection()
			{
				powerUpTimer = 5.0f;
				state = PowerUpState::SPEEDPOWER;
			}

		protected:
			bool movingForward;
			bool movingBackwards;
			bool movingLeft;
			bool movingRight;
			bool isSprinting;
			bool isJumping;
			bool isGrounded;

			PxVec3 fwd;
			PxVec3 right;
			float score;
			float speed;
			float raycastTimer;
			float powerUpTimer;

			PowerUpState state;
			int coinsCollected;
			float jumpHeight;
			float finishTime;
			bool finished;
		};
	}
}
