/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Transform Implementation		 */
#include "Transform.h"
#include "../../Common/PhyxConversions.h"

using namespace NCL::CSC8503;

Transform::Transform()
{

}

Transform::~Transform()
{
}

void Transform::UpdateMatrix()
{
	matrix = Matrix4::Translation(Vector3(pxPos)) *
		Matrix4(Quaternion(pxOrientation)) *
		Matrix4::Scale(Vector3(pxScale));


	//matrix = Matrix4::Translation(position) *
		          //Matrix4(orientation) *
		          //Matrix4::Scale(scale);
}

Transform& Transform::SetPosition(const Vector3& worldPos)
{
	pxPos = PhyxConversions::GetVector3(worldPos);
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetScale(const Vector3& worldScale)
{
	pxScale = PhyxConversions::GetVector3(worldScale);
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetOrientation(const Quaternion& worldOrientation)
{
	pxOrientation = PhyxConversions::GetQuaternion(worldOrientation);
	UpdateMatrix();
	return *this;
}


Transform& Transform::SetPosition(const PxVec3& worldPos)
{
	pxPos = worldPos;
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetScale(const PxVec3& worldScale)
{
	pxScale = worldScale;
	UpdateMatrix();
	return *this;
}

Transform& Transform::SetOrientation(const PxQuat& worldOrientation)
{
	pxOrientation = worldOrientation;
	UpdateMatrix();
	return *this;
}