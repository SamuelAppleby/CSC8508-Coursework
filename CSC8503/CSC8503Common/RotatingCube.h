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

				body->addTorque(PxVec3(rotationAxes->x * dt, rotationAxes->y * dt, rotationAxes->z * dt), PxForceMode::eFORCE);
			}

		protected:
			const PxVec3* rotationAxes;
			PxRigidDynamic* body;

		};
	}
}

