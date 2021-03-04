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

			void SetPosition(const PxVec3& worldPos);
			void SetScale(const PxVec3& worldScale);
			void SetOrientation(const PxQuat& newOr);

			PxVec3 GetPosition() const
			{
				return pxPos;
			}

			PxVec3 GetScale() const
			{
				return pxScale;
			}

			PxQuat GetOrientation() const
			{				
				return pxOrientation;
			}

			Matrix4 GetMatrix() const
			{
				return matrix;
			}
			void UpdateMatrix();
		protected:
			Matrix4	matrix;	
			PxVec3 pxPos;
			PxVec3 pxScale;
			PxQuat pxOrientation;
		};
	}
}

