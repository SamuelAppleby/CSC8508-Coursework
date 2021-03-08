/*			  Created By Rich Davison
*			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game World Definition		 */
#pragma once
#include <vector>
#include <functional>
#include "GameObject.h"
#include "../../Common/Camera.h"
#include <algorithm>
#include <random>
#include "Debug.h"
#include "../CSC8503Common/GameObject.h"
#include "../../Common/Camera.h"
#include "../../Common/Vector2.h"
#include "../../Common/Vector3.h"
#include "../../Common/TextureLoader.h"
#include "../../Common/Matrix4.h"
namespace NCL {
	class Camera;
	namespace CSC8503 {
		class GameObject;
		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;
		class GameWorld : public PxSimulationEventCallback {
		public:
			GameWorld();
			~GameWorld();

			/* PhysX callback methods */
			void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs) override {
				PxActor* actor1 = pairHeader.actors[0];
				PxActor* actor2 = pairHeader.actors[1];
				FindObjectFromPhysicsBody(actor1)->OnCollisionBegin(FindObjectFromPhysicsBody(actor2));
				FindObjectFromPhysicsBody(actor2)->OnCollisionBegin(FindObjectFromPhysicsBody(actor1));
			}
			void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
			void onWake(PxActor** actors, PxU32 count) {}
			void onSleep(PxActor** actors, PxU32 count) {}
			void onTrigger(PxTriggerPair* pairs, PxU32 count) {}
			void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);

			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			void ShowFacing();

			virtual void UpdateWorld(float dt);

			static GameObject* FindObjectFromPhysicsBody(PxActor* actor);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(GameObjectIterator& first, GameObjectIterator& last) const;

			bool GetShuffleObjects() const {
				return shuffleObjects;
			}
			static std::vector<GameObject*> gameObjects;
		protected:
			Camera* mainCamera;
			bool	shuffleObjects;
			int		worldIDCounter;
		};
	}
}

