#pragma once
#include "GameObject.h"
#include "Cannonball.h"
namespace NCL {
	namespace CSC8503 {
		class Cannon :
			public GameObject {
		public:
			Cannon(PxVec3* newPosition, PxVec3* newTrajectory, int newShotTimes = 10, int newShotSize = 5, int newDensity = 5) {
				shotDensity = newDensity;
				shotSize = newShotSize > 0 ? newShotSize : 5;
				shotTimes = newShotTimes;
				trajectory = newTrajectory;
				position = new PxTransform(*newPosition);
				cannonBallPosition = new PxVec3(newPosition->x + ((abs(trajectory->x) / trajectory->x) * 9), newPosition->y, newPosition->z);
				SetName("Cannon");

			}

			~Cannon() {

				SetRenderObject(NULL);
				SetPhysicsObject(NULL);
				/*
				if (physicsObject != nullptr) {
					delete physicsObject;
				}
				if (renderObject != nullptr) {
					delete renderObject;
				}
				*/
			}

			void Update(float dt) {
				timeSinceShot += dt;

			}

			float getTimeSinceShot() {
				return timeSinceShot;
			}

			PxVec3* shoot() {
				timeSinceShot = 0;
				return trajectory;
			}

			PxTransform* getCannonBallPosition() {
				return new PxTransform(*cannonBallPosition);
			}

			PxTransform* getPosition() {
				return position;
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

			void setBody(PxActor* newBody) {
				body = newBody;
			}


			PxActor* getBody() {
				return body;
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
			PxVec3* cannonBallPosition;
			PxTransform* position = new PxTransform(0, 0, 0);
			PxVec3* trajectory;
			PxActor* body;
		};
	}
}


