#pragma once
#include "GameObject.h"
#include "../../Common/Win32Window.h"
#include "../CSC8503Common/PhysxConversions.h"
#include "../CSC8503Common/PxPhysicsSystem.h"

namespace NCL{
	namespace CSC8503 {
		class PlayerObject : public GameObject {
			float MAX_SPEED = 50.0f;
		public:
			PlayerObject();

			void Update(float dt) override;
			void FixedUpdate(float fixedDT) override;

			
			void SetIsGrounded(bool val) {
				isGrounded = val;
			}
			
			void SetRaycastTimer(float val)  {
				raycastTimer = val;
			}

			float GetRaycastTimer() const {
				return raycastTimer;
			}

			void CollectCoin() {
				coinsCollected++;
			}

			int GetCoinsCollected() const {
				return coinsCollected;
			}

			float GetPowerUpTimer() const
			{
				return powerUpTimer;
			}

			void LongJumpColelction() {
				powerUpTimer = 5.0f;
			}


			//TEMPORARY FIX FOR OBSTACLES
			void OnCollisionBegin(GameObject* otherObject) {
				if (otherObject->GetName()._Equal("Floor")) {
					isGrounded = true;
				}
			}

			void OnCollisionEnd(GameObject* otherObject) {
				if (otherObject->GetName()._Equal("Floor")) {
					isGrounded = false;
				}
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

			int coinsCollected;
			float jumpHeight;
		};
	}
}
