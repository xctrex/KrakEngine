/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: WinMain.cpp
Purpose: Main class, Runs the engine
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "Core.h"
#include "WindowSystem.h"
#include "GraphicsSystem.h"
#include "PhysicsSystem.h"
#include "GamePlaySystem.h"
#include "ControllerSystem.h"
#include "InputSystem.h"
#include "Factory.h"
#include "Serializer.h"
#include "SoundSystem.h"
#include "AISystem.h"
#include <io.h>
#include <Fcntl.h>

using namespace KrakEngine;

const int ClientWidth = 800;
const int ClientHeight = 600;
const bool FULL_SCREEN = true;

//The entry point for the application--called automatically when the game starts.
//The first parameter is a handle to the application instance.
//The second parameter is the previous app instance which you can use to prevent being launched multiple times
//The third parameter is the command line string, but use GetCommandLine() instead.
//The last parameter is the manner in which the application's window is to be displayed (not needed).
INT WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance, LPSTR, INT)
{
	{
	//Create the core engine which manages all the systems that make up the game
	Core* engine = new Core();
#if defined(_DEBUG)
	AllocConsole();
	
	HANDLE handle_out = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long) handle_out, _O_TEXT);
	FILE* hf_out = _fdopen(hCrt, "w");
	setvbuf(hf_out, NULL, _IONBF, 1);
	*stdout = *hf_out;

	HANDLE handle_in = GetStdHandle(STD_INPUT_HANDLE);
	hCrt = _open_osfhandle((long) handle_in, _O_TEXT);
	FILE* hf_in = _fdopen(hCrt, "r");
	setvbuf(hf_in, NULL, _IONBF, 128);
	*stdin = *hf_in;
#endif	
	//Initialize the game by creating the various systems needed and adding them to the core engine
	WindowSystem* windows = new WindowSystem(hInst,ClientWidth, ClientHeight,FULL_SCREEN);
	GraphicsSystem* graphics = new GraphicsSystem(windows);
	PhysicsSystem* physics = new PhysicsSystem();
	InputSystem* input = new InputSystem(windows);
	SoundSystem * audio = new SoundSystem();
	AISystem * AI = new AISystem();
	Factory * factory  = new Factory();
	Gameplay * gameplay = new Gameplay();
	ControllerSystem * controller = new ControllerSystem();
			
	engine->AddSystem(audio);
	engine->AddSystem(windows);

	engine->AddSystem(graphics);
	engine->AddSystem(physics);
	engine->AddSystem(input);
	engine->AddSystem(AI);
	engine->AddSystem(factory);
	engine->AddSystem(gameplay);
	engine->AddSystem(controller);

	engine->InitializeSystems();

	//Run the game
	engine->UpdateSystems();

	//Delete all the systems
	engine->DestroySystems();

	//Delete the engine itself
	delete engine;

	}

	//Game over, application will now close
	return 0;
}