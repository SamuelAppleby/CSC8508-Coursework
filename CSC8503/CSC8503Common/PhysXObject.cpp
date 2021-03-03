/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Physics Object Implementation		 */
#include "PhysXObject.h"
using namespace NCL;
using namespace CSC8503;

PhysXObject::PhysXObject(PxRigidActor* p) {
	pXActor = p;
}

PhysXObject::~PhysXObject() {

}
