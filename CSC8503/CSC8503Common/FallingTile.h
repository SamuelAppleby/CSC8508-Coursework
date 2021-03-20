#pragma once
#include "GameObject.h"
#include "GameWorld.h"
namespace NCL {
	namespace CSC8503 {
		class FallingTile : public GameObject {

		public:

			FallingTile(string newName, float newTime = 10, float newResetY = 0, PxVec3 newOriginalPos = PxVec3(0,200,0)) {
				name = newName;
				originalTime = newTime;
				timeAlive = 0;
				resetY = newResetY;
				originalPos = newOriginalPos;
			}
			void ResetTile(){
				std::cout << physicsObject->GetPXActor()->getGlobalPose().p.y << "\n";
				SetPosition(PxVec3(0,200,0));

				/*
				timeAlive = 0;
				started = false;
				PxRigidDynamic* actor = (PxRigidDynamic*)physicsObject->GetPXActor();
				actor->setLinearVelocity(PxVec3(0, 0, 0));
				actor->setAngularVelocity(PxVec3(0, 0, 0));
				actor->setGlobalPose(PxTransform(originalPos));
				*/
				//physicsObject->GetPXActor()->setGlobalPose(PxTransform(originalPos));
				//renderObject->GetTransform()->SetPosition(originalPos);
			}

			void Update(float dt) override {
				/*
				if (started) {
					timeAlive += dt;

					if (timeAlive >= originalTime) {
						this->GetPhysicsObject()->GetPXActor()->setActorFlag(PxActorFlag::eDISABLE_GRAVITY, false);
					}
				}
				*/
				//if (this->GetTransform().GetPosition().y < resetY) {
				//	ResetTile();
				//}
				
			}

			void OnCollisionBegin(GameObject* otherObject) override {
				//if (otherObject->GetName()._Equal("Player")) {
				//	started = true;
				//}
			}


		protected:
			bool started;
			float originalTime;
			float timeAlive;
			float resetY;
			PxVec3 originalPos;
		};
	}
}


