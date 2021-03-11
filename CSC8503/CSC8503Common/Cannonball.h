#pragma once
#include "GameObject.h"
#include "GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class Cannonball :
			public GameObject {
		public:
			Cannonball(PxActor* newBody) {
				body = newBody;
				SetName("Cannonball");
			}
			void Update(float dt) {
				StepTimeAlive(dt);
			}

			void setDestroy(bool newDestroy) {
				destroy = newDestroy;
			}

			bool getDestroy() {
				return destroy;
			}

			PxActor* getBody() {

				return body;
			}



		protected:
			bool destroy;
			PxActor* body;
		};
	}
}
