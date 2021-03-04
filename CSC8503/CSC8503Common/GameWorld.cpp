/*			Created By Rich Davison
 *			Edited By Samuel Buzz Appleby
 *               21/01/2021
 *                170348069
 *			Game World Implementation		 */
#include "GameWorld.h"
using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()
{
	mainCamera = new Camera();
	shuffleObjects		= false;
	worldIDCounter		= 0;
}

GameWorld::~GameWorld()
{
}

void GameWorld::Clear()
{
	gameObjects.clear();
}

void GameWorld::ClearAndErase()
{
	for (auto& i : gameObjects)
	{
		delete i;
	}
	Clear();
}

void GameWorld::AddGameObject(GameObject* o)
{
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
}

void GameWorld::RemoveGameObject(GameObject* o, bool andDelete)
{
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete)
		delete o;
}

void GameWorld::GetObjectIterators(GameObjectIterator& first, GameObjectIterator& last) const
{
	first = gameObjects.begin();
	last = gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f)
{
	for (GameObject* g : gameObjects)
	{
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt)
{
	std::random_device rd;
	std::mt19937 g(rd());
	if (shuffleObjects)
		std::shuffle(gameObjects.begin(), gameObjects.end(), g);
	for (auto& i : gameObjects) {
	    i->GetTransform().SetOrientation(i->GetPhysicsObject()->GetPXActor()->getGlobalPose().q);
		i->GetTransform().SetPosition(i->GetPhysicsObject()->GetPXActor()->getGlobalPose().p);
		i->Update(dt);
	}
}

void GameWorld::ShowFacing() {
	for (auto& i : gameObjects) {
		Debug::DrawAxisLines(i->GetTransform().GetMatrix(), 2.0f);		// Show the axes of all active game objects
	}
}


