/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Tutorial Game definition		 */
#pragma once
#include "../CSC8503Common/PhysxConversions.h"
#include "../CSC8503Common/CollisionDetection.h"
#include "../GameTech/GameManager.h"

namespace NCL
{
	namespace CSC8503
	{
		const int IDEAL_FRAMES = 240;
		const float IDEAL_DT = 1.0f / IDEAL_FRAMES;
		class PlayerObject;
		enum class FinishType { INGAME, TIMEOUT, WIN, LOSE };
		class LevelCreator
		{
		public:
			LevelCreator();
			~LevelCreator();

			void ResetWorld();

			virtual void Update(float dt);
			virtual void UpdatePhysics(float dt);

			void UpdateLevel(float dt);
			void UpdatePlayer(float dt);
			void InitPlayer(const PxTransform& t, const PxReal scale);
			void virtual InitWorld(LevelState state);

		protected:
			float	dTOffset;
			int realFrames;
			float fixedDeltaTime;
			LevelState currentLevel;
			void InitCamera();
			void UpdateKeys();

			void InitFloors(LevelState state);
			void InitGameExamples(LevelState state);
			void InitGameObstacles(LevelState state);

			bool SelectObject();
			void DebugObjectMovement();
		};
	}
}

