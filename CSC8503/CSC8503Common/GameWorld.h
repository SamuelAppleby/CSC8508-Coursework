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
#include "../Common/Camera.h"
#include "../Common/Vector2.h"
#include "../Common/Vector3.h"
#include "../Common/TextureLoader.h"
#include "../Common/Matrix4.h"
#include "../Common/Light.h"

//constexpr auto NUM_OF_LIGHTS = 32;

namespace NCL
{
	class Camera;
	namespace CSC8503
	{
		class GameObject;
		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;
		class GameWorld
		{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void RemoveGameObject(GameObject* o, bool andDelete = false);


		
			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleObjects(bool state)
			{
				shuffleObjects = state;
			}

			void ShowFacing();

			virtual void UpdateWorld(float dt);

			GameObject* FindObjectFromPhysicsBody(PxRigidActor* actor);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(GameObjectIterator& first, GameObjectIterator& last) const;

			int GetTotalWorldObjects() const
			{
				return gameObjects.size();
			}

			bool GetShuffleObjects() const
			{
				return shuffleObjects;
			}

			void IncreamentLightCount()
			{
				++lightCount;
			}
			int GetLightCount()
			{
				return lightCount;
			}

			void AddLight(Light& l)
			{
				//lights[lightCount] = l;
			}

		protected:
			std::vector<GameObject*> gameObjects;
			//Light lights[NUM_OF_LIGHTS];
			Camera* mainCamera;
			bool	shuffleObjects;
			int		worldIDCounter;
			int     lightCount;
		};
	}
}

