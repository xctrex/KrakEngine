/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Core.h
Purpose: Header for Core.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once

#include "ISystem.h"
#include <vector>

#include "GameStateManager.h"

namespace KrakEngine{

	class Core{
	
	public:
		Core();
		~Core();

		bool InitializeSystems();
		bool ShutDownSystems();
		void AddSystem(ISystem* system);
		void UpdateSystems();		
		void DestroySystems();
		void Broadcast(Message* m);

		//wrappers around GameStateManager
		void SetNextState(GameStates::GameState next);
		GameStates::GameState GetCurrentState();
		int GetCurrentLevel();
		void SetNextLevel(int level);
		int GetNextLevel();
		void ChangeLevel();

	private:
		std::vector<ISystem*> Systems;
		unsigned LastUpdate;
		GameStateManager GameStateManager;
	};
	extern Core* CORE;
}