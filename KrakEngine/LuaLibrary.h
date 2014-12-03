/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: LuaLibrary.h
Purpose: header for LuaLibrary.h
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/19/14
- End Header -----------------------------------------------------*/

#pragma once

#include "Lua\lua.hpp"


namespace KrakEngine
{
	class GameObject;
	class LuaInterpreter;

class LuaLibrary
{
public:
	LuaLibrary(LuaInterpreter * parent);
	~LuaLibrary() {};

	static int cpp_Create(lua_State * Lstate);
	static int cpp_CreateAt(lua_State * Lstate);
	static int cpp_SetCurrentObj(lua_State * Lstate);
	static int cpp_GetCurrentObj(lua_State * Lstate);

	static int cpp_ImportCurrentTransform(lua_State *Lstate);
	static int cpp_ImportCurrentImage(lua_State *Lstate);

	//change component data memebers
	static int cpp_ChangeSize(lua_State *Lstate);
	static int cpp_ChangeVelocity(lua_State *Lstate);
	static int cpp_ChangeMass(lua_State *Lstate);
	static int cpp_ChangeRotation(lua_State *Lstate);
	static int cpp_ChangeAction(lua_State *Lstate);
	static int cpp_SetState(lua_State *Lstate);
	static int cpp_SetLeftDestination(lua_State * Lstate);
	static int cpp_SetRightDestination(lua_State * Lstate);
	static int cpp_ChangeTexture(lua_State *Lstate);
	static int cpp_ChangeAudioFile(lua_State * Lstate);

	//add a component
	static int cpp_AddTimer(lua_State *Lstate);
	static int cpp_AddAudio(lua_State * Lstate);
	static int cpp_AddSpawner(lua_State *Lstate);

	//check time
	static int cpp_GetTime(lua_State *Lstate);

	//mime box mechanix
	static int cpp_AddPointToBox(lua_State *Lstate);
	static int cpp_PlaceBox(lua_State *Lstate);


	static int cpp_AddToLevelEditorList(lua_State * Lstate);

	static void SetCurrentObject( GameObject * go ) {CurrentObj = go;}
	static GameObject * GetCurrentObject() {return CurrentObj;}

	//Print to Console
	static int cpp_Print(lua_State *Lstate);

	//Functions for Audio
	static int cpp_PlayAudioOne(lua_State * Lstate);
	static int cpp_PlayAudioTwo(lua_State * Lstate);
	static int cpp_FadeAudioObjPosition(lua_State * Lstate);
	static int cpp_FadeAudioPlayerPosition(lua_State * Lstate);
	static int cpp_FadeAudioPlayerTrain(lua_State * Lstate);

	//Functions for Triggers
	static int cpp_ContinueTrigger(lua_State *Lstate);

	static int cpp_PanCameraUp(lua_State *Lstate);
	static int cpp_PanCameraDown(lua_State *Lstate);

	static int cpp_GoToNextLevel(lua_State *Lstate);
	static int cpp_GoToVictory(lua_State *Lstate);

	static int cpp_SelectControlledObj(lua_State *Lstate);
	static int cpp_Debug(lua_State *Lstate);

	static int cpp_GetObjectId(lua_State *Lstate);
	static int cpp_SetCurrentObjByID(lua_State *Lstate);

	static int cpp_GetDirection(lua_State *Lstate);

	static int cpp_SetImageFile(lua_State *Lstate);

	static int cpp_SetCameraDepth(lua_State *Lstate);

	static int cpp_ToggleFree(lua_State *Lstate);
	//Functions for Particles
	static int cpp_ImportCurrentParticle(lua_State *Lstate);
	static int cpp_AddParticleTimer(lua_State *Lstate);
	static int cpp_SetParticleCenter(lua_State * Lstate);
	static int cpp_SetParticleScript(lua_State * Lstate);





	static int cpp_ButtonPressed(lua_State *Lstate);

private:
	static LuaInterpreter * m_Parent;

	static GameObject * CurrentObj;
};
}