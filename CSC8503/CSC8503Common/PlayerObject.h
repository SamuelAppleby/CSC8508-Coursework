#pragma once
#include "GameObject.h"

namespace NCL{
	namespace CSC8503 {
		class PlayerObject : public GameObject {
		public:
			PlayerObject();

			void Update(float dt) override;
			bool CheckGrounded();
		protected:
			float score;
			float speed;
			bool isGrounded;
			float raycastTimer;
		};
	}
}
