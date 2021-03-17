#pragma once
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../../Common/Window.h"
#include "LevelCreator.h"

using namespace NCL;
using namespace CSC8503;

LevelCreator* tutorialGame = nullptr;
//int winner = 0;
//int playerScore = 0;
//int aiScore = 0;

class Pause : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		tutorialGame->GetRenderer()->Render();
		Debug::FlushRenderables(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		GameManager::SetLevelState(LevelState::PAUSED);
	}
};

class Level1 : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		tutorialGame->Update(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
		{
			*newState = new Pause();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			GameManager::ResetMenu();
			return PushdownResult::Pop;
		}

		//if (tutorialGame->GetWinner() != 0)
		//{
		//	winner = tutorialGame->GetWinner();
		//	playerScore = tutorialGame->GetPlayerScore();
		//	aiScore = tutorialGame->GetAIScore();

		//	return PushdownResult::Pop; // back to main menu
		//}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		GameManager::GetAudioManager()->StopSound();
		GameManager::GetAudioManager()->PlayAudio("../../Assets/Audio/Level1Music.mp3", true);
		GameManager::SetLevelState(LevelState::LEVEL1);
		//winner = 0;
	}
};

class Level2 : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		tutorialGame->Update(dt);
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
		{
			*newState = new Pause();
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{
			GameManager::ResetMenu();
			return PushdownResult::Pop;
		}
		//if (tutorialGame->GetWinner() != 0)
		//{
		//	winner = tutorialGame->GetWinner();
		//	playerScore = tutorialGame->GetPlayerScore();
		//	aiScore = tutorialGame->GetAIScore();

		//	return PushdownResult::Pop; // back to main menu
		//}
		return PushdownResult::NoChange;
	}

	void OnAwake() override
	{
		GameManager::GetAudioManager()->StopSound();
		GameManager::GetAudioManager()->PlayAudio("../../Assets/Audio/Level2Music.mp3", true);
		GameManager::SetLevelState(LevelState::LEVEL2);
		//winner = 0;
	}
};

class MainMenu : public PushdownState
{
public:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		//tutorialGame->Update(dt);
		tutorialGame->GetRenderer()->Update(dt);
		tutorialGame->GetRenderer()->Render();

		//g->UpdateGame(dt);

		/*if (winner != 0)
		{
			if (winner == 1)
			{

				Debug::Print("Player Won!!!!!!!!", Vector2(30, 20), Vector4(1, 1, 1, 1));
				Debug::Print("Player Score:" + std::to_string(playerScore), Vector2(30, 25), Vector4(1, 1, 1, 1));
				Debug::Print("AI Score:    " + std::to_string(aiScore), Vector2(30, 30), Vector4(1, 1, 1, 1));
			}
			else
			{
				Debug::Print("AI Won!!!!!!!!!", Vector2(30, 20), Vector4(1, 1, 1, 1));
				Debug::Print("Player Score:" + std::to_string(playerScore), Vector2(30, 25), Vector4(1, 1, 1, 1));
				Debug::Print("AI Score:    " + std::to_string(aiScore), Vector2(30, 30), Vector4(1, 1, 1, 1));
			}
		}*/

		Debug::FlushRenderables(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD1))
		{
			*newState = new Level1();

			//playerScore = aiScore = 0;
			tutorialGame->InitWorld(LevelState::LEVEL1);
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD2))
		{
			*newState = new Level2();

			//playerScore = aiScore = 0;
			tutorialGame->InitWorld(LevelState::LEVEL2);
			//tutorialGame->InitAI();
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM3) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD3))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}

	void  OnAwake() override
	{
		GameManager::SetLevelState(LevelState::MENU);
		if (tutorialGame == nullptr)
		{
			tutorialGame = new LevelCreator();
		}
		else {
			tutorialGame->ResetWorld();
		}
		GameManager::GetAudioManager()->StopSound();
		GameManager::GetAudioManager()->PlayAudio("../../Assets/Audio/MenuMusic.mp3", true);

		//Debug::Print("Press Space To  Start", Vector2(50, 50), Vector4(1, 0, 0, 1));
		//std::cout << " Welcome to a really awesome game !\n";
		//std::cout << " Press Space To Begin or escape to quit !\n";
	}
};


