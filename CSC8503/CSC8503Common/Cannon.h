#pragma once
#include "GameObject.h"
#include "Cannonball.h"
namespace NCL {
	namespace CSC8503 {
		class Cannon :
			public GameObject {
		public:
			Cannon(PxVec3 newTrajectory, int newShotTimes = 10, int newShotSize = 5) {
				shotSize = newShotSize;
				shotTimes = newShotTimes;
				trajectory = newTrajectory;
			}

			~Cannon() {
				SetRenderObject(NULL);
				SetPhysicsObject(NULL);
			}

			void Update(float dt) {
				GameObject::Update(dt);
				timeSinceShot += dt;
			}

			float getTimeSinceShot() {
				return timeSinceShot;
			}

			PxVec3 shoot() {
				timeSinceShot = 0;
				return trajectory;
			}

			void addShot(Cannonball* shot) {
				shots.push_back(shot);
			}

			void removeShot(Cannonball* shot) {
				shots.erase(std::remove(shots.begin(), shots.end(), shot), shots.end());

			}

			vector<Cannonball*> getShots() {
				return shots;
			}

			int getMaxAlive() {
				return shotTimes;
			}

			int getShotSize() {
				return shotSize;
			}

			int getShotDensity() {
				return shotDensity;
			}
		protected:
			int shotDensity;
			int shotSize;
			int shotTimes;
			float timeSinceShot = 0;
			vector<Cannonball*> shots;
			PxVec3 trajectory;
		};
	}
}


