/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GameStateManager.h
Purpose: Header for GameStateManager.h
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 1/25/14
- End Header -----------------------------------------------------*/

#pragma once

namespace KrakEngine{

	namespace GameStates {
		
		enum GameState{
			Entry,
			InGame,
			Quit,
			Restart,
			GameOver,
			Victory,
			VictoryScene,
			LevelEdit,
			StartGame,
			StartMenu,
			InMenu,
			LoadingInitial,
			LoadingRestart,
			Digipen,
			GoNinja,
			ControllerRequired,
			HowToPlay,
			SceneOne,
			SceneTwo,
			SceneThree,
			SceneFour
		};
	}

	class GameStateManager
	{
	public:
		GameStateManager() : m_currentState(GameStates::Digipen), m_nextState(GameStates::Digipen), m_currentLevel(0) {};
		~GameStateManager() {};

		//get current state
		GameStates::GameState GetCurrentState() const {return m_currentState; };

		void SetNextState(GameStates::GameState next) {m_nextState = next; };

		void ChangeState() { m_currentState = m_nextState;};
		void ChangeLevel() { m_currentLevel = m_nextLevel;};
				
		//access/mutate level
		int GetCurrentLevel() const {return m_currentLevel; };

		int GetNextLevel() const { return m_nextLevel; };
		void SetNextLevel(int level) { m_nextLevel = level;};



	private:
	
		GameStates::GameState m_currentState;
		GameStates::GameState m_nextState;
		int m_currentLevel;
		int m_nextLevel;
	};

}