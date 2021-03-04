/*			 Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Transform Definition		 */
#pragma once
#include "../../Common/Matrix4.h"
#include "../../Common/Matrix3.h"
#include "../../Common/Vector3.h"
#include "../../Common/Quaternion.h"
 //#include "../CSC8503Common/PxPhysicsSystem.h"
#include "../../include/PxPhysicsAPI.h"

#include <vector>

using std::vector;

using namespace NCL::Maths;
using namespace physx;

namespace NCL
{
	namespace CSC8503
	{
		class Transform
		{
		public:
			Transform();
			~Transform();

			Transform& SetPosition(const Vector3& worldPos);
			Transform& SetScale(const Vector3& worldScale);
			Transform& SetOrientation(const PxQuat& newOr);


			Transform& SetPosition(const PxVec3& worldPos);
			Transform& SetScale(const PxVec3& worldScale);
			Transform& SetOrientation(const Quaternion& newOr);

			Vector3 GetPosition() const
			{
				return Vector3(pxPos);
			}

			Vector3 GetScale() const
			{
				return Vector3(pxScale);
			}

			Quaternion GetOrientation() const
			{				
				return Quaternion(pxOrientation);
			}

			Matrix4 GetMatrix() const
			{
				return matrix;
			}
			void UpdateMatrix();
		protected:
			Matrix4		matrix;		


			PxVec3    pxPos;
			PxVec3    pxScale;
			PxQuat    pxOrientation;
		};
	}
}

