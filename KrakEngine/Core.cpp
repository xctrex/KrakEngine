/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Core.cpp
Purpose: Definition of Engine Core
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "Core.h"
#include<time.h>   
#include <Windows.h>
#include "Factory.h"

namespace KrakEngine{

	Core* CORE;

	Core::Core(){
		LastUpdate = 0;
		CORE = this;
	}

	Core::~Core(){}

	void Core::AddSystem(ISystem* system){
		Systems.push_back(system);
	}

	bool Core::InitializeSystems(){
		for (unsigned i = 0; i < Systems.size(); i++){
			if (!Systems[i]->Initialize())
				return false;
		}
		return true;
	}

	void Core::UpdateSystems(){
		LastUpdate = clock();

		while(GameStateManager.GetCurrentState() != GameStates::Quit){
			unsigned current = clock();
			float dt = min(0.1f, (current-LastUpdate) / 1000.0f);

			LastUpdate = current;
			for (unsigned i = 0; i<Systems.size(); i++)
				Systems[i]->Update(dt);
			
			GameStateManager.ChangeState();
			Sleep(1);
		}

		g_FACTORY->DeleteAllObjects();
	}

	bool Core::ShutDownSystems(){
		for (unsigned i = 0; i < Systems.size(); i++)
			if (!Systems[i]->Shutdown())
				return false;
		return true;
	}

	void Core::DestroySystems(){
	
		for (unsigned i = 0; i < Systems.size(); i++)
			delete Systems[Systems.size()-1 - i];
	}

	void Core::Broadcast(Message* message){
		for (unsigned i = 0; i < Systems.size(); i++)
			Systems[i]->HandleMessages(message);
	}

	void Core::SetNextState(GameStates::GameState next)
	{ 
		GameStateManager.SetNextState(next); 
	}

	GameStates::GameState Core::GetCurrentState()
	{ 
		return GameStateManager.GetCurrentState(); 
	}

	int Core::GetCurrentLevel()
	{
		return GameStateManager.GetCurrentLevel();
	}

	void Core::SetNextLevel(int level)
	{
		GameStateManager.SetNextLevel(level);
	}

	int Core::GetNextLevel()
	{
		return GameStateManager.GetNextLevel();
	}
	
	void Core::ChangeLevel()
	{
		GameStateManager.ChangeLevel();
	}
}
