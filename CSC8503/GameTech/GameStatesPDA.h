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

LevelCreator* levelCreator = nullptr;
//int winner = 0;
//int playerScore = 0;
//int aiScore = 0;

class Pause : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		GameManager::GetAudioManager()->UpdateAudio(dt);
		GameManager::GetRenderer()->Render();
		Debug::FlushRenderables(dt);

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE) || 
			GameManager::GetRenderer()->GetUIState() == UIState::MENU || 
			GameManager::GetRenderer()->GetUIState() == UIState::INGAME)
		{
			return PushdownResult::Pop;
		}
		return PushdownResult::NoChange;
	}
	void OnAwake() override
	{
		GameManager::GetWindow()->LockMouseToWindow(false);
		GameManager::GetWindow()->ShowOSPointer(true);
		GameManager::GetRenderer()->SetUIState(UIState::PAUSED);
	}
};

class Level : public PushdownState
{
	PushdownResult OnUpdate(float dt, PushdownState** newState) override
	{
		if (GameManager::GetRenderer()->GetUIState() == UIState::MENU)
		{
			GameManager::ResetMenu();
			return PushdownResult::Pop;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE))
		{		
			*newState = new Pause();
			return PushdownResult::Push;
		}

		levelCreator->Update(dt);
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
		if (GameManager::GetRenderer()->GetUIState() != UIState::MENU)
		{
			GameManager::GetWindow()->LockMouseToWindow(true);
			GameManager::GetWindow()->ShowOSPointer(false);
			//GameManager::GetAudioManager()->StopSound();
			if (GameManager::GetLevelState() == LevelState::LEVEL1) 
				GameManager::GetAudioManager()->PlayAudio("../../Assets/Audio/Level1Music.mp3", true);
			else 
				GameManager::GetAudioManager()->PlayAudio("../../Assets/Audio/Level2Music.mp3", true);
			GameManager::GetRenderer()->SetUIState(UIState::INGAME);
		}		
		//winner = 0;
	}
};

class MainMenu : public PushdownState {
public:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		GameManager::GetRenderer()->Render();
		GameManager::GetAudioManager()->UpdateAudio(dt);
		Debug::FlushRenderables(dt);

		switch (GameManager::GetRenderer()->GetUIState()) {
		case UIState::QUIT:
			return PushdownResult::Pop;
			break;
		case UIState::SOLOLEVEL1:
			*newState = new Level();
			GameManager::GetAudioManager()->StopSound();
			levelCreator->InitWorld(LevelState::LEVEL1);
			GameManager::SetLevelState(LevelState::LEVEL1);
			return PushdownResult::Push;
			break;
		case UIState::SOLOLEVEL2:
			*newState = new Level();
			GameManager::GetAudioManager()->StopSound();
			levelCreator->InitWorld(LevelState::LEVEL2);
			GameManager::SetLevelState(LevelState::LEVEL2);
			return PushdownResult::Push;
			break;
		case UIState::SOLOLEVEL3:
			*newState = new Level();
			GameManager::GetAudioManager()->StopSound();
			levelCreator->InitWorld(LevelState::LEVEL3);
			GameManager::SetLevelState(LevelState::LEVEL3);
			return PushdownResult::Push;
			break;
		case UIState::HOSTLEVEL1:
			/* Code for hosting level 1 */
			break;
		case UIState::JOINLEVEL1:
			/* Code for joining a hosted level 1 */
			IPAddress = GameManager::GetRenderer()->GetIP();
			portNo = GameManager::GetRenderer()->GetPort();
			break;
		case UIState::HOSTLEVEL2:
			/* Code for hosting level 2 */
			break;
		case UIState::JOINLEVEL2:
			/* Code for joining a hosted level 2 */
			IPAddress = GameManager::GetRenderer()->GetIP();
			portNo = GameManager::GetRenderer()->GetPort();
			break;
		case UIState::HOSTLEVEL3:
			/* Code for hosting level 2 */
			break;
		case UIState::JOINLEVEL3:
			/* Code for joining a hosted level 2 */
			IPAddress = GameManager::GetRenderer()->GetIP();
			portNo = GameManager::GetRenderer()->GetPort();
			break;
		}
		return PushdownResult::NoChange;
	}
private:
	string IPAddress;
	string portNo;
	void  OnAwake() override {
		GameManager::GetWindow()->ShowOSPointer(true);
		GameManager::GetWindow()->LockMouseToWindow(false);
		if (levelCreator == nullptr) {
			levelCreator = new LevelCreator();
		}
		else {
			levelCreator->ResetWorld();
		}
		GameManager::GetAudioManager()->StopSound();
		GameManager::GetAudioManager()->PlayAudio("../../Assets/Audio/MenuMusic.mp3", true);
	}
};


