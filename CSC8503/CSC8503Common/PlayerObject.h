#pragma once
#include "GameObject.h"
#include "../../Common/Win32Window.h"
#include "../CSC8503Common/PhysxConversions.h"
namespace NCL{
	namespace CSC8503 {
		class PlayerObject : public GameObject {
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
		protected:
			bool movingForward = false, movingBackwards = false, movingLeft = false, movingRight = false, isJumping = false;
			Vector3 fwd;
			Vector3 right;
			float score;
			float speed;
			bool isGrounded;
			float raycastTimer;
			int coinsCollected;
		};
	}
}
