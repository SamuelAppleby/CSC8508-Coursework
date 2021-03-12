#pragma once
#include "../CSC8503/CSC8503Common/GameObject.h"


namespace NCL {
	namespace CSC8503 {
		class RotatingCube :
			public GameObject {
		public:

			RotatingCube(PxRigidDynamic* newBody, const PxVec3* newRotationAxes) {
				rotationAxes = newRotationAxes;
				body = newBody;
				SetName("RotatingCube");
			}
			~RotatingCube() {
				delete rotationAxes;
				delete physicsObject;
				delete renderObject;
			};

			PxVec3 getRotationAxes() {
				return *rotationAxes;
			}

			void Update(float dt) {
				body->setAngularVelocity(PxVec3((rotationAxes->x * 10) * dt, (rotationAxes->y *10) * dt, (rotationAxes->z * 10) * dt), PxForceMode::eFORCE);
				//body->setKinematicTarget(PxTransform())

			}

			
			void OnCollisionBegin(GameObject* otherObject) {

				if (otherObject->GetName() == "Player") {
					otherObject->SetGrounded(true);
				}


			}

			void OnCollisionEnd(GameObject* otherObject) {

				if (otherObject->GetName() == "Player") {
					otherObject->SetGrounded(false);
				}


			}
			

		protected:
			const PxVec3* rotationAxes;
			PxRigidDynamic* body;

		};
	}
}

