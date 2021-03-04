#pragma once
#include"Quaternion.h"
#include"Vector3.h"
#include "../../include/PxPhysicsAPI.h"

using namespace NCL::Maths;
using namespace physx;

static class PhyxConversions
{
public:
	static PxVec3 GetVector3(Vector3 v)
	{
		return PxVec3(v.x, v.y, v.z);
	

	static PxQuat GetQuaternion(Quaternion q)
	{
		return PxQuat(q.x, q.y, q.z, q.w);
	}
};