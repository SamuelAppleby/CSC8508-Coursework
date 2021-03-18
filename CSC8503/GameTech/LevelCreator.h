/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game definition		 */
#pragma once
#include "../CSC8503Common/PhysxConversions.h"
#include "../CSC8503/CSC8503Common/CollisionDetection.h"
#include "../GameTech/GameManager.h"
#include "GameTechRenderer.h"

namespace NCL
{
	namespace CSC8503
	{
		class PlayerObject;
		enum class FinishType { INGAME, TIMEOUT, WIN, LOSE };
		class LevelCreator
		{
		public:
			LevelCreator();
			~LevelCreator();

			void ResetWorld();

			void Update(float dt);
			void UpdateLevel(float dt);

			void InitWorld(LevelState state);

			GameTechRenderer* GetRenderer() const {
				return renderer;
			}

		protected:
			GameTechRenderer* renderer;
			LevelState currentLevel;
			void InitCamera();
			void UpdateKeys();

			void InitFloors(LevelState state);
			void InitGameExamples(LevelState state);
			void InitGameObstacles(LevelState state);

			bool SelectObject();
			void DebugObjectMovement();
			void LockedObjectMovement(float dt);

			PlayerObject* player;

			//Level2 stuff here
			void updateCannons(float dt);
			void updateCannonBalls();
			void clearCannons();
		};
	}
}

