#pragma once
#include "../CSC8503Common/PushdownState.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../../Common/Window.h"
#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

TutorialGame* tutorialGame = nullptr;
//int winner = 0;
//int playerScore = 0;
//int aiScore = 0;

class Pause : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		//tutorialGame->Update(dt);
		tutorialGame->renderer->Render();

		Debug::Print("Press P To Resume", Vector2(20, 50), Vector4(1, 0, 0, 1));
		Debug::FlushRenderables(dt);


		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::P))
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		Debug::Print("Press Space To  Start", Vector2(5, 50), Vector4(1, 0, 0, 1));
		std::cout << "Paused";
	}
};

class SinglePlayer : public PushdownState
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
		//winner = 0;
	}
};

class Multiplayer : public PushdownState
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
		//winner = 0;
	}
};

class MainMenu : public PushdownState
{

	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		//tutorialGame->Update(dt);
		tutorialGame->renderer->Update(dt);
		tutorialGame->renderer->Render();

		//g->UpdateGame(dt);
		Debug::Print("1: Single Player", Vector2(5, 60), Vector4(1, 0, 0, 1));
		Debug::Print("2: Vs bot", Vector2(5, 70), Vector4(1, 0, 0, 1));
		Debug::Print("3: Exit", Vector2(5, 80), Vector4(1, 0, 0, 1));

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
			*newState = new SinglePlayer();

			//playerScore = aiScore = 0;
			tutorialGame->InitWorld();
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2) ||
			Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUMPAD2))
		{
			*newState = new Multiplayer();

			//playerScore = aiScore = 0;
			tutorialGame->InitWorld();
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
		if (tutorialGame == nullptr)
		{
			tutorialGame = new TutorialGame();
		}
		else {
			tutorialGame->ResetWorld();
		}

		//Debug::Print("Press Space To  Start", Vector2(50, 50), Vector4(1, 0, 0, 1));
		//std::cout << " Welcome to a really awesome game !\n";
		//std::cout << " Press Space To Begin or escape to quit !\n";
	}
};


