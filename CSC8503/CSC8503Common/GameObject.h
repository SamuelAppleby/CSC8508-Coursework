/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game Object Definition		 */
#pragma once
#include <vector>
#include "Transform.h"
#include "RenderObject.h"
#include "PhysXObject.h"

using std::vector;
namespace NCL {
	namespace CSC8503 {
		class GameObject {
		public:
			GameObject(string name = "");
			~GameObject();

			virtual void Update(float dt) {
				
			}

			void SetName(string val) {
				name = val;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysXObject* GetPhysicsObject() const {
				return physicsObject;
			}

			virtual void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			virtual void SetPhysicsObject(PhysXObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {}

			void SetWorldID(int newID) {
				worldID = newID;
			}

			int GetWorldID() const {
				return worldID;
			}

			int GetSelected() const {
				return selectedObject;
			}

			void SetSelected(bool val) {
				selectedObject = val;
			}

			void IncreaseTimeInSet(float dt) {
				timeInSet += dt;
			}

			void SetTimeInSet(float val) {
				timeInSet = val;
			}

			float GetTimeInSet() const {
				return timeInSet;
			}

			void SetPowerUpTimer(float val) {
				powerUpTimer = val;
			}

			float GetPowerUpTimer() const {
				return powerUpTimer;
			}

			float GetTimeAlive() const {
				return timeAlive;
			}

			void StepTimeAlive(float dt) {
				timeAlive += dt;
			}

		protected:
			Transform transform;

			PhysXObject* physicsObject;
			RenderObject* renderObject;

			int	worldID;
			string	name;
			bool selectedObject;
			float timeInSet;
			float timeAlive;
			float powerUpTimer;
		};
	}
}

