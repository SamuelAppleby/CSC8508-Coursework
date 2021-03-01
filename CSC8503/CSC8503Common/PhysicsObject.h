/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Physics Object Definition		 */
#pragma once
#include "../../Common/Vector3.h"
#include "../../Common/Matrix3.h"

#include <ctype.h>
#include "../../include/PxPhysicsAPI.h"
#include "../../include/foundation/Px.h"
#include "../../include/foundation/PxTransform.h"
using namespace physx;

using namespace NCL::Maths;

namespace NCL {

	namespace CSC8503 {
		class Transform;

		class PhysicsObject {
		public:
			PhysicsObject(Transform* parentTransform, PxRigidActor* pxTrans);
			~PhysicsObject();

			void SetElasticity(float val) {
				elasticity = val;
			}

			float GetElasticity() const {
				return elasticity;
			}

			void SetFriction(float val) {
				friction = val;
			}

			float GetFriction() const {
				return friction;
			}

			Vector3 GetLinearVelocity() const {
				return linearVelocity;
			}

			Vector3 GetAngularVelocity() const {
				return angularVelocity;
			}

			Vector3 GetTorque() const {
				return torque;
			}

			Vector3 GetForce() const {
				return force;
			}

			void SetInverseMass(float invMass) {
				inverseMass = invMass;
			}

			float GetInverseMass() const {
				return inverseMass;
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);

			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);
			void AddForceAtLocalPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);

			void ClearForces();

			void SetLinearVelocity(const Vector3& v) {
				linearVelocity = v;
			}

			void SetAngularVelocity(const Vector3& v) {
				angularVelocity = v;
			}

			void InitCubeInertia();
			void InitSphereInertia(bool hollow);
			void InitCapsuleInertia();

			void UpdateInertiaTensor();

			Matrix3 GetInertiaTensor() const {
				return inverseInteriaTensor;
			}

			void SetIsStatic(bool val) {
				isStatic = val;
			}
			bool GetIsStatic() const {
				return isStatic;
			}
			void SetIsAsleep(bool val) {
				isAsleep = val;
			}
			bool GetIsAsleep() const {
				return isAsleep;
			}
			PxRigidActor* GetPXActor() const {
				return pXActor;
			}
		protected:
			PxRigidActor* pXActor;
			Transform* transform;
			float inverseMass;
			float elasticity;
			float friction;

			//linear stuff
			Vector3 linearVelocity;
			Vector3 force;


			//angular stuff
			Vector3 angularVelocity;
			Vector3 torque;
			Vector3 inverseInertia;
			Matrix3 inverseInteriaTensor;

			bool isAsleep;
			bool isStatic;

		};
	}
}

