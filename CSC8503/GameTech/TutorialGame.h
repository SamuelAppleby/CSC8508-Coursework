/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game definition		 */
#pragma once
#include "../GameTech/GameTechRenderer.h"
#include "../CSC8503Common/StateGameObject.h"
#include "../CSC8503Common/NavigationGrid.h"
#include "../CSC8503Common/PathFindingEnemyStateGameObject.h"
#include "../CSC8503Common/PatrolEnemyStateGameObject.h"
#include "../CSC8503Common/PlatformStateGameObject.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/Camera.h"
#include "../CSC8503Common/Pushdownstate.h"
#include "../CSC8503Common/PauseScreen.h"
#include "../CSC8503Common/BehaviourTreeEnemy.h"
#include <ctype.h>
#include "PxPhysicsAPI.h"
#include "../CSC8503Common/PxPhysicsSystem.h"
#include "../GameTech/WorldCreator.h"

namespace NCL {
	namespace CSC8503 {
		class PlayerObject;
		enum class FinishType { INGAME, TIMEOUT, WIN, LOSE };
		class TutorialGame {
		public:
			TutorialGame();
			~TutorialGame();

			void Update(float dt);
			void UpdateLevel(float dt);
			void DrawDebugInfo();

		protected:
			void InitCamera();
			void UpdateKeys();

			void InitWorld();
			void InitFloors(int level);
			void InitGameExamples(int level);
			void InitGameObstacles(int level);
			
			bool SelectObject();
			void DebugObjectMovement();
			void LockedObjectMovement(float dt);

			GameTechRenderer* renderer;
			PxPhysicsSystem* pXPhysics;
			GameWorld* world;

			bool useGravity;
			bool useBroadphase;
			bool inSelectionMode;
			float forceMagnitude;
			float reloadTime;
			float timeOut;
			GameObject* selectionObject = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
		
			PlayerObject* player;
			vector<EnemyStateGameObject*> enemies;
			vector<PlatformStateGameObject*> platforms;
			int currentLevel;
			bool lockedOrientation;

			float textSize = 15.0f;

			int avgFps;
			int framesPerSecond;
			float fpsTimer;

			CameraState camState;
			vector<GameObject*> projectiles;
			FinishType finish;

			vector<BehaviourTreeEnemy*> behaviourEnemies;

			GameObject* currentEnemy = nullptr;
		};
	}
}

