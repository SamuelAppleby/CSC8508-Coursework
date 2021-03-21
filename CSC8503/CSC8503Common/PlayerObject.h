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

			void SetIsGrounded(bool val) {
				isGrounded = val;
			}
			bool IsGrounded() const	{
				return isGrounded;
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
			//float score;
			float speed;
			bool isGrounded;
			float raycastTimer;
			int coinsCollected;
		};
	}
}
