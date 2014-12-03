/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: LuaInterpreter.cpp
Purpose: A basic Lua interpreter that allows for basic communication between C++ and Lua scripts.
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/7/14
- End Header -----------------------------------------------------*/

#include "LuaInterpreter.h"
#include "LuaLibrary.h"

#include <iostream>

namespace KrakEngine
{
	LuaLibrary * LuaInterpreter::lualib = NULL;

	//constructor
	//begins lua state, opens lua libraries.
	//--------------------------------------------------------------------------------
	LuaInterpreter::LuaInterpreter()
	{
		LuaState = luaL_newstate();
		luaL_openlibs(LuaState);

		lualib = new LuaLibrary(this);

		InitializeRegisterFunctions();
	}

	//destructor
	//ends LuaState
	//--------------------------------------------------------------------------------
	LuaInterpreter::~LuaInterpreter()
	{
		lua_close(LuaState);

		delete lualib;
	}


	//--------------------------------------------------------------------------------
	void LuaInterpreter::InitializeRegisterFunctions()
	{
		RegisterFunction("Create", &LuaLibrary::cpp_Create);
		RegisterFunction("CreateAt", &LuaLibrary::cpp_CreateAt);
		RegisterFunction("ChangeSize", &LuaLibrary::cpp_ChangeSize);
		RegisterFunction("GetTransform", &LuaLibrary::cpp_ImportCurrentTransform);
		RegisterFunction("ChangeVelocity", &LuaLibrary::cpp_ChangeVelocity);
		RegisterFunction("ChangeMass", &LuaLibrary::cpp_ChangeMass);
		RegisterFunction("AddTimer", &LuaLibrary::cpp_AddTimer);
		RegisterFunction("SetCurrentObject", &LuaLibrary::cpp_SetCurrentObj);
		RegisterFunction("ChangeRotation", &LuaLibrary::cpp_ChangeRotation);
		RegisterFunction("ChangeAction", &LuaLibrary::cpp_ChangeAction);
		RegisterFunction("SetState", &LuaLibrary::cpp_SetState);
		RegisterFunction("SetLeftDestination", &LuaLibrary::cpp_SetLeftDestination);
		RegisterFunction("SetRightDestination", &LuaLibrary::cpp_SetRightDestination);
		RegisterFunction("AddPointToBox", &LuaLibrary::cpp_AddPointToBox);	
		RegisterFunction("AddAudio", &LuaLibrary::cpp_AddAudio);
		RegisterFunction("ChangeAudioFile", &LuaLibrary::cpp_ChangeAudioFile);
		RegisterFunction("ChangeTexture", &LuaLibrary::cpp_ChangeTexture);
		RegisterFunction("AddToLevelEditorList", &LuaLibrary::cpp_AddToLevelEditorList);
		RegisterFunction("SetLeftDestination", &LuaLibrary::cpp_SetLeftDestination);
		RegisterFunction("SetRightDestination", &LuaLibrary::cpp_SetRightDestination);
		RegisterFunction("AddSpawner", &LuaLibrary::cpp_AddSpawner);
		RegisterFunction("GetTime", &LuaLibrary::cpp_GetTime);
		RegisterFunction("GetImage", &LuaLibrary::cpp_ImportCurrentImage);
		RegisterFunction("Print", &LuaLibrary::cpp_Print);
		RegisterFunction("Continue", &LuaLibrary::cpp_ContinueTrigger);
		RegisterFunction("PanCameraUp", &LuaLibrary::cpp_PanCameraUp);
		RegisterFunction("PanCameraDown", &LuaLibrary::cpp_PanCameraDown);
		RegisterFunction("GoToNextLevel", &LuaLibrary::cpp_GoToNextLevel);
		RegisterFunction("Debug", &LuaLibrary::cpp_Debug);

		RegisterFunction("SelectCurrentControlledObject", &LuaLibrary::cpp_SelectControlledObj);

		RegisterFunction("GetObjID", &LuaLibrary::cpp_GetObjectId);
		RegisterFunction("SetCurrentObjByID", &LuaLibrary::cpp_SetCurrentObjByID);
		RegisterFunction("PlayAudio1", &LuaLibrary::cpp_PlayAudioOne);
		RegisterFunction("PlayAudio2", &LuaLibrary::cpp_PlayAudioTwo);
		RegisterFunction("FadeAudioObjPosition", &LuaLibrary::cpp_FadeAudioObjPosition);
		RegisterFunction("FadeAudioPlayerPosition", &LuaLibrary::cpp_FadeAudioPlayerPosition);
		RegisterFunction("FadeAudioPlayerTrain", &LuaLibrary::cpp_FadeAudioPlayerTrain);

		RegisterFunction("SetImage", &LuaLibrary::cpp_SetImageFile);

		RegisterFunction("GetDirection", &LuaLibrary::cpp_GetDirection);

		RegisterFunction("GetParticle", &LuaLibrary::cpp_ImportCurrentParticle);
		RegisterFunction("AddParticleTimer", &LuaLibrary::cpp_AddParticleTimer);
		RegisterFunction("SetCenter", &LuaLibrary::cpp_SetParticleCenter);
		RegisterFunction("SetParticleScript", &LuaLibrary::cpp_SetParticleScript);
	
		RegisterFunction("PlaceBox", &LuaLibrary::cpp_PlaceBox);
	
		RegisterFunction("GoToVictory", &LuaLibrary::cpp_GoToVictory);
		RegisterFunction("SetCameraDepth", &LuaLibrary::cpp_SetCameraDepth);
		
		RegisterFunction("ButtonPressed", &LuaLibrary::cpp_ButtonPressed);

		RegisterFunction("SetFreeCamera", &LuaLibrary::cpp_ToggleFree);
	}

	//LoadScript
	//>>>Resets the stack<<<
	//and runs a lua script of the name given in filename
	//--------------------------------------------------------------------------------
	bool LuaInterpreter::LoadScript(std::string filename)
	{
		SetTop();
		std::string file = "Assets/Scripts/" + filename + ".lua";

		const char * ccfile = file.c_str();
		return (luaL_dofile(LuaState, ccfile) == 0);
	}


	//RegisterFunction
	//registers a LuaFunction of type lua_CFunction with a certain name given by the parameter function
	//--------------------------------------------------------------------------------
	void LuaInterpreter::RegisterFunction(const char * function, lua_CFunction func )
	{
		lua_register(LuaState, function, func);	
	}

	//SetTop
	//Resets the stack pointer to point at 0.
	//--------------------------------------------------------------------------------
	void LuaInterpreter::SetTop()
	{
		lua_settop(LuaState, 0);
		CurrentStackPosition = 0;
	}

	void LuaInterpreter::SetCurrentObj(GameObject * go)
	{
		lualib->SetCurrentObject(go);
	}
};