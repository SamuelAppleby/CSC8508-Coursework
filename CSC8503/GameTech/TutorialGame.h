/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game definition		 */
#pragma once
#include "../GameTech/GameTechRenderer.h"
#include "../CSC8503Common/GameWorld.h"
#include "../../Common/Camera.h"
#include "../../Common/PhyxConversions.h"
#include "../CSC8503/CSC8503Common/CollisionDetection.h"
#include "../CSC8503Common/PxPhysicsSystem.h"
#include "../GameTech/WorldCreator.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerObject;
		enum class FinishType { INGAME, TIMEOUT, WIN, LOSE };
		class TutorialGame
		{
		public:
			TutorialGame();
			~TutorialGame();

			void ResetWorld();

			void Update(float dt);
			void UpdateLevel(float dt);

			void InitWorld(int currentLevel);
			GameTechRenderer* renderer;

		protected:
			void InitCamera();
			void UpdateKeys();

			void InitFloors(int level);
			void InitGameExamples(int level);
			void InitGameObstacles(int level);

			bool SelectObject();
			void DebugObjectMovement();
			void LockedObjectMovement(float dt);

			GameWorld* world;
			PxPhysicsSystem* pXPhysics;

			PlayerObject* player;

			//Level2 stuff here
			vector<Cannon*> cannons;
			void updateCannons(float dt);
			void updateCannonBalls();
			void clearCannons();
		};
	}
}

