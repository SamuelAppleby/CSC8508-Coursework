#pragma once
#include "GameObject.h"
#include "../CSC8503Common/PhysxConversions.h"

enum class LevelState;

namespace NCL{
	namespace CSC8503 {
		class PlayerObject : public GameObject {
		public:
			PlayerObject();

			void Update(float dt) override;

			bool CheckHasFinished(LevelState state);

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
			void SetFinished(bool val) {
				finished = val;
			}
			bool HasFinished() const {
				return finished;
			}
			void SetFinishTime(float time) {
				finishTime = time;
			}
			float GetFinishTime() const {
				return finishTime;
			}

		protected:
			//float score;
			float speed;
			bool isGrounded;
			float raycastTimer;
			int coinsCollected;
			float finishTime;
			bool finished;
		};
	}
}
