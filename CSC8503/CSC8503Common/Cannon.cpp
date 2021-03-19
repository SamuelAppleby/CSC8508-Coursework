#include "Cannon.h"
using namespace NCL;
using namespace CSC8503;

void Cannon::Update(float dt)
{

	GameObject::Update(dt);
	timeSinceShot += dt;


	if (timeSinceShot >= 5)
	{
		Shoot();
	}
}

void Cannon::Shoot()
{
	for (auto& i : GameManager::GetObstacles()->cannons)
	{
		if (!i->isActive)
		{
			currentBall = i;

			break;
		}
	}

	if (currentBall != nullptr)
	{
		//;
		currentBall->ResetBall(transform.GetPosition() + translate, trajectory);
	}
	timeSinceShot = 0;
}
