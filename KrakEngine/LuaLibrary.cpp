/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: LuaLibrary.cpp
Purpose: A library of all functions that can be interprted by lua into our engine.
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/19/14
- End Header -----------------------------------------------------*/

#include "LuaLibrary.h"
#include "LuaInterpreter.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Model.h"
#include "GameObject.h"
#include "Timer.h"
#include "AIComp.h"
#include "Action.h"
#include "AIComp.h"
#include "Audio.h"
#include "Sprite.h"
#include "Factory.h"
#include "GamePlaySystem.h"
#include "Spawner.h"
#include "Image.h"
#include "Camera.h"
#include "Particle.h"


namespace KrakEngine
{
	GameObject * LuaLibrary::CurrentObj = NULL;

	LuaInterpreter * LuaLibrary::m_Parent = NULL;

	XMFLOAT3 FindClosetPoint(XMFLOAT3 pt1, XMFLOAT3 pt2, XMFLOAT3 goalPt);

	float GetDistance(XMFLOAT3 pt1, XMFLOAT3 pt2);

	LuaLibrary::LuaLibrary(LuaInterpreter * parent)
	{
		m_Parent = parent;
	}
		
	//cpp_Create
	//C++ wrapper to convert Create from factory class into a Lua function.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_Create(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 2)
			return 0;

		std::string s = lua_tostring(Lstate, 1);
		std::string s2 = lua_tostring(Lstate, 2);

		SetCurrentObject(g_FACTORY->Create(s));

		CurrentObj->SetType(s2);

		return 0;
	}

	//cpp_CreateAt
	//C++ wrapper around Create that allows placement with transform component.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_CreateAt(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 5)
			return 0;

		std::string s = lua_tostring(Lstate, 1);
		std::string s2 = lua_tostring(Lstate, 2);
		float x = (float)lua_tonumber(Lstate, 3);
		float y = (float)lua_tonumber(Lstate, 4);
		float z = (float)lua_tonumber(Lstate, 5);

		XMFLOAT3 pos(x, y, z);

		GameObject * go = g_FACTORY->Create(s);
		if(go)
		{
			SetCurrentObject(go);
			Transform * tc = go->has(Transform);
			if(tc)
				tc->SetPosition(pos);
			Image *ic = go->has(Image);
			if(ic)
				ic->SetPosition(pos);
		}

		CurrentObj->SetType(s2);

		return 0;
	}

	//cpp_ChangeSize
	//C++ wrapper around Model.SetSize() and Physics Component's size, allows the change of size.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeSize(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if (n != 3)
			return 0;

		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);	
		float z = (float)lua_tonumber(Lstate, 3);

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Component * c = go->has(Model);
			if(c)
			{
				Model * mc = dynamic_cast<Model *>(c);
				mc->SetSize(x, y, z);
			}
			c = go->has(RigidBody)
				if(c)
				{
					RigidBody * pc = dynamic_cast<RigidBody *>(c);
					pc->SetSize(x,y,z);
				}

			Sprite * sc = go->has(Sprite)
			if(sc)
			{
				sc->SetSize(x,y);
			}

			Image *ic = go->has(Image)
			if(ic)
			{
				ic->SetSize(x,y);
			}

		}
		return 0;
	};

	//cpp_SetCurrentObj
	//The Current Object is the one being acted upon by Lua functions. This changes it to an object of a given name (Must be unique).
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_SetCurrentObj(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;

		std::string s = lua_tostring(Lstate, 1);

		GameObject * go = g_FACTORY->GetObjList().FindByName(s);
		if(!go)
			return 0;

		SetCurrentObject(go);

		return 0;
	}


	//cpp_ImportCurrentTransform
	//This function imports the transform component of the current object to a Lua script.
	//This allows for an object's position to be known even if it is not the current object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ImportCurrentTransform(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 0)
			return 0;

		if(!CurrentObj)
			return 0;

		Transform * tc = CurrentObj->has(Transform);
		if(!tc)
			return 0;

		lua_pushnumber(Lstate, tc->GetPosition().x);
		lua_pushnumber(Lstate, tc->GetPosition().y);
		lua_pushnumber(Lstate, tc->GetPosition().z);

		return 3;
	}
	
	//cpp_ImportCurrentImage
	//This function imports the image component of the current object to a Lua script.
	//This allows for an object's position to be known even if it is not the current object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ImportCurrentImage(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 0)
			return 0;

		if(!CurrentObj)
			return 0;

		Image * ic = CurrentObj->has(Image);
		if(!ic)
			return 0;

		lua_pushnumber(Lstate, ic->GetPosition().x);
		lua_pushnumber(Lstate, ic->GetPosition().y);

		return 2;
	}
	
	//cpp_ChangeVelocity
	//Changes the velocity x,y
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeVelocity(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 2)
			return 0;
		
		float x = (float)lua_tonumber(Lstate, 1);	
		float y = (float)lua_tonumber(Lstate, 2);	
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			RigidBody * pc = go->has(RigidBody);
			if(pc)
			{
				pc->velocity.x = x;
				pc->velocity.y = y;
				pc->velocity.z = 0;
			}
		}
		return 0;
	}

	//cpp_AddTimer
	//Adds a DeathTimer to the component (when timer ends, object dies).
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_AddTimer(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 1)
			return 0;

		float time = (float)lua_tonumber(Lstate, 1);	

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Timer * timer = go->has(Timer);

			if(timer
				&& dynamic_cast<DeathTimer*>(timer))
				return 0;

			Timer * t = new DeathTimer(time);

			go->AddComponent(t);
		}

		return 0;
	}

	//cpp_AddParticleTimer
	//Adds a DeathTimer to the component (when timer ends, object dies).
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_AddParticleTimer(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 2)
			return 0;

		float time = (float)lua_tonumber(Lstate, 1);
		int id = (int)lua_tonumber(Lstate, 2);

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Timer * timer = go->has(Timer);

			if(timer
				&& dynamic_cast<ParticleTimer*>(timer))
				return 0;

			Timer * t = new ParticleTimer(time, id);

			go->AddComponent(t);
		}

		return 0;
	}

	//cpp_AddAudio
	//Adds an audio component to the object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_AddAudio(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 0)
			return 0;

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Audio * a = new Audio();

			go->AddComponent(a);
		}

		return 0;
	}

	//cpp_PlayAudioOne
	//Plays the action 1 audio file
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_PlayAudioOne(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 0)
			return 0;

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Audio* a = go->has(Audio);
			if(a){
				a->playAction1();
			}

		}
		return 0;
	}

	//cpp_PlayAudioTwo
	//Plays the action 2 audio file
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_PlayAudioTwo(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 0)
			return 0;

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Audio* a = go->has(Audio);
			if(a){
				a->playAction2();
			}

		}

		return 0;
	}

	//cpp_FadeAudioObjPosition
	//Fades the audio based on the trigger owner's position
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_FadeAudioObjPosition(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 0)
			return 0;

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Audio* a = go->has(Audio);
			Trigger* t = go->has(Trigger);
			Transform* tr = go->has(Transform);
			if(a){
				if(tr){
					if(t){
						XMFLOAT3 start = t->GetStartFocus();
						XMFLOAT3 end = t->GetEndFocus();
						XMFLOAT3 pos = tr->GetPosition();
						XMFLOAT3 closet = FindClosetPoint(start, end, pos);
						float maxDistance;
						if(start.x - closet.x < end.x - closet.x){
							maxDistance = GetDistance(start, t->GetStartPosition());
						}else{
							maxDistance = GetDistance(end, t->GetEndPosition());
						}
						float volume = max( 0, a->getVolume()- a->getVolume()/maxDistance * GetDistance(closet, pos) );
						if(volume >= 0){
							a->setVolume(volume);
						}else{
							a->setVolume(0.0f);
						}
				}
				}
			}

		}

		return 0;
	}

	//cpp_FadeAudioPlayerPosition
	//Fades the audio based on the player's position
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_FadeAudioPlayerPosition(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 0)
			return 0;

		GameObject * go = GetCurrentObject();
		GameObject * go2 = g_CONTROLLERSYSTEM->FindController();
		if(go)
		{
			Audio* a = go->has(Audio);
			Trigger* t = go->has(Trigger);
			Transform* tr = go2->has(Transform);
			if(a){
				if(tr){
					if(t){
						XMFLOAT3 start = t->GetStartFocus();
						XMFLOAT3 end = t->GetEndFocus();
						XMFLOAT3 pos = tr->GetPosition();
						XMFLOAT3 closet = FindClosetPoint(start, end, pos);
						float volume = (float) min( 1.0, 1.0 / GetDistance(pos, closet) );
						if(volume >= 0){
							a->setVolume(volume);
						}else{
							a->setVolume(0.0f);
						}
				}
				}
			}

		}

		return 0;
	}

	//cpp_FadeAudioPlayerTrain
	//This function is for use only with the subway in the beginning of level one.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_FadeAudioPlayerTrain(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
		
		if(n != 0)
			return 0;

		GameObject * go = GetCurrentObject();
		GameObject * go2 = g_CONTROLLERSYSTEM->FindController();
		if(go)
		{
			Audio* a = go->has(Audio);
			Trigger* t = go->has(Trigger);
			Transform* tr = go2->has(Transform);
			if(a){
				if(tr){
					if(t){
						XMFLOAT3 start = t->GetStartFocus();
						XMFLOAT3 end = t->GetEndFocus();
						end.x = 25;
						XMFLOAT3 pos = tr->GetPosition();
						XMFLOAT3 closet = FindClosetPoint(start, end, pos);
						float volume = (float) min( 1.0, 1.0 / GetDistance(pos, closet) );
						if(volume >= 0){
							a->setVolume(volume);
						}else{
							a->setVolume(0.0f);
						}
				}
				}
			}

		}

		return 0;
	}

	//cpp_AddSpawner
	//Adds a spawner component to the object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_AddSpawner(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 2)
			return 0;

		std::string name = lua_tostring(Lstate, 1);
		int time = (int)lua_tonumber(Lstate, 2);	

		GameObject * go = GetCurrentObject();
		if(go)
		{
			Spawner * s = new Spawner(time, 0, 1000, name);

			go->AddComponent(s);
		}

		return 0;
	}


	//cpp_ChangeAudioFile
	//Changes the current object's audio file (takes a number then a file).
	//0 = jump
	//1 = action1
	//2 = action2
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeAudioFile(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 2)
			return 0;
		
		int action = (int)lua_tonumber(Lstate, 1);	
		std::string filename = lua_tostring(Lstate, 2);	
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			Audio * ac = go->has(Audio);
			if(ac)
			{
				if(action == 0)
					ac->setJump(g_SOUNDSYSTEM->findBuffer(filename));
				else if(action == 1)
					ac->setAction1(g_SOUNDSYSTEM->findBuffer(filename));
				else if(action == 2)
					ac->setAction2(g_SOUNDSYSTEM->findBuffer(filename));
			}
		}
		return 0;
	}

	//cpp_ChangeMass
	//Changes the current object's mass (float)
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeMass(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;
		
		float newmass = (float)lua_tonumber(Lstate, 1);	
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			RigidBody * pc = go->has(RigidBody);
			if(pc)
			{
				pc->mass = newmass;
			}
		}
		return 0;
	}

	//cpp_ChangeTexture
	//Changes the current objcet's texture (string)
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeTexture(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;
		
		std::string newtexture = lua_tostring(Lstate, 1);	
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			Sprite * sc = go->has(Sprite);
			if(sc)
			{
				sc->SetTextureName(newtexture);
			}
		}
		return 0;
	}

	//cpp_AddToLevelEditorList
	//
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_AddToLevelEditorList(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);
			if(n != 0) return 0;

		std::string objtypename = GetCurrentObject()->GetType();

		g_GAMEPLAYSYSTEM->EmplaceNewObjectOnLevelEditorList(objtypename);

		std::string name = "Assets/Objects/" + objtypename;

		Serializer Seri;
		if(Seri.LoadFile(name))
			Seri.ClearFile();

		Seri.WriteGameObjectToFile(GetCurrentObject());
		
		Seri.SaveFile();

		return 0;
	}
	
	//cpp_ChangeRotation
	//Changes the current object's rotation (x,y,z)
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeRotation(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 3)
			return 0;

		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);
		float z = (float)lua_tonumber(Lstate, 3);

		XMFLOAT3 pos(x, y, z);

		GameObject * go = GetCurrentObject();

		Transform * tc = go->has(Transform);
		if(tc)
			tc->SetRotation(pos);
		

		return 0;
	}

	//cpp_ChangeAction
	//Changes the script file that will be read for a specific action (int, string)
	//int = action
	//string = file name
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ChangeAction(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 2)
			return 0;
		
		int action = (int)lua_tonumber(Lstate, 1);	
		std::string newaction = lua_tostring(Lstate, 2);

		GameObject * go = GetCurrentObject();
		Action * a = go->has(Action);
		if(!a)
			return 0;

		if(action == 1)
			a->SetAction1(newaction);
		else if(action == 2)
			a->SetAction2(newaction);

		return 0;
	}

	//cpp_SetState
	//Changes the state of the current object's ai component (string)
	//-------------------------------------------------------------------
	int LuaLibrary::cpp_SetState(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;
		
		std::string newstate = lua_tostring(Lstate, 1);	
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			AI * aic = go->has(AI);
			if(aic)
			{
				aic->setState(g_AISYSTEM->serializeState(newstate));
				aic->setStatus(onEntry);
			}
		}
		return 0;
	}

	//cpp_SetLeftDestination
	//Changes the current object's ai component's left destination (float, float, float)
	//-------------------------------------------------------------------
	int LuaLibrary::cpp_SetLeftDestination(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 3)
			return 0;
		
		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);
		float z = (float)lua_tonumber(Lstate, 3);
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			AI * aic = go->has(AI);
			if(aic)
			{
				aic->setLeftDestination(XMFLOAT3(x, y, z));
			}
		}
		return 0;
	}

	//cpp_SetRightDestination
	//Changes the current object's ai component's right destination (float, float, float)
	//-------------------------------------------------------------------
	int LuaLibrary::cpp_SetRightDestination(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 3)
			return 0;
		
		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);
		float z = (float)lua_tonumber(Lstate, 3);
		
		GameObject * go = GetCurrentObject();
		if(go)
		{
			AI * aic = go->has(AI);
			if(aic)
			{
				aic->setRightDestination(XMFLOAT3(x, y, z));
			}
		}
		return 0;
	}

	//cpp_AddPointToBox
	//For the mime's draw box mechanic, adds the point (x,y,z) to the box, then gives Lua the
	//curent index to count the number of points.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_AddPointToBox(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 3)
			return 0;
		
		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);

		XMFLOAT3 pos = g_GAMEPLAYSYSTEM->GetMechanics().AddPointToBox(x,y);

		lua_pushnumber(Lstate, pos.x);
		lua_pushnumber(Lstate, pos.y);

		return 2;
	}

	//GetTime
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_GetTime(lua_State *Lstate)
	{
		GameObject * go = GetCurrentObject();
		Timer * t = go->has(Timer);

		lua_pushnumber(Lstate, t->GetTime());

		return 1;
	}

	//Print
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_Print(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;
		
		std::string str = lua_tostring(Lstate, 1);

		std::cout << str << std::endl;

		return 0;

	}

	//Continue Trigger
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ContinueTrigger(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;
		
		GameObject* go = GetCurrentObject();
		bool cont = (bool)lua_toboolean(Lstate, 1);

		if(go){
			Trigger* tri = go->has(Trigger);

			if(tri){
				tri->SetRun(!cont);
			}
		}

		return 0;
	}

	//Pan Camera
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_PanCameraUp(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
 		if (n != 2)
			return 0;

		float yposition = (float)lua_tonumber(Lstate, 1);
		float height = (float)lua_tonumber(Lstate, 2);
		
		Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

		c->SetTargetHeight(height);
		c->SetTargetLookHeight(yposition);

		c->SetPan(Camera::Pan_Up);

		bool keepgoing = true;

		lua_pushboolean(Lstate, keepgoing);

		return 1;
	}



	//Pan Camera
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_PanCameraDown(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
 		if (n != 2)
			return 0;

		float yposition = (float)lua_tonumber(Lstate, 1);
		float height = (float)lua_tonumber(Lstate, 2);

 		Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

		c->SetTargetHeight(height);
		c->SetTargetLookHeight(yposition);

		c->SetPan(Camera::Pan_Down);

		lua_pushboolean(Lstate, true);

		return 1;
	}



	int LuaLibrary::cpp_SelectControlledObj(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 0)
			return 0;

		CurrentObj = g_CONTROLLERSYSTEM->FindController();
		return 0;
	}

	int LuaLibrary::cpp_Debug(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 0)
			return 0;

		//uncomment to set break point in lua
		//int debug = 0;

		return 0;
	}

	//GoToNextLevel
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_GoToNextLevel(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 0)
			return 0;

		CORE->SetNextLevel(CORE->GetCurrentLevel() + 1);
		
		return 0;
	}

	int LuaLibrary::cpp_GoToVictory(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 0)
			return 0;

		CORE->SetNextState(GameStates::Victory);
		
		return 0;
	}

	int LuaLibrary::cpp_GetObjectId(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 0)
			return 0;

		int id = CurrentObj->GetID();

		lua_pushnumber(Lstate, id);
		return 1;
	}

	int LuaLibrary::cpp_SetCurrentObjByID(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 1)
			return 0;

		int number = (int)lua_tonumber(Lstate, 1);

		CurrentObj = g_FACTORY->FindByID(number);

		return 0;
	}


	int LuaLibrary::cpp_GetDirection(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 0)
			return 0;

		RigidBody * rb = CurrentObj->has(RigidBody);

		lua_pushnumber(Lstate, rb->direction);

		return 1;
	}

	//PlaceBox
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_PlaceBox(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 2)
			return 0;
				
		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);

		g_GAMEPLAYSYSTEM->GetMechanics().PlaceBox(x,y);

		return 0;

	}

	//SetImageFile
	//Sets the file to use for the image component of the current object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_SetImageFile(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);
		if(n != 1)
			return 0;

		std::string newimagename = lua_tostring(Lstate, 1);

		Image * c = CurrentObj->has(Image);
		if(!c) return 0;

		c->SetImageName(newimagename);

		return 0;
	}


	XMFLOAT3 FindClosetPoint(XMFLOAT3 pt1, XMFLOAT3 pt2, XMFLOAT3 goalPt){
		XMFLOAT3 MPoint = XMFLOAT3();
		MPoint.x = (goalPt.x < pt1.x) ? pt1.x : (goalPt.x > pt2.x) ? pt2.x : goalPt.x;
		MPoint.y = (goalPt.y < pt1.y) ? pt1.y : (goalPt.y > pt2.y) ? pt2.y : goalPt.y;
		MPoint.z = (goalPt.z < pt1.z) ? pt1.z : (goalPt.z > pt2.z) ? pt2.z : goalPt.z;
		return MPoint;
	}

	float GetDistance(XMFLOAT3 pt1, XMFLOAT3 pt2){
		return sqrt(pow((pt2.x - pt1.x), 2) + pow((pt2.y - pt1.y), 2));
	}

	//cpp_ImportCurrentParticle
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ImportCurrentParticle(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 0)
			return 0;

		if(!CurrentObj)
			return 0;

		Particle * tc = CurrentObj->has(Particle);
		RigidBody * t = CurrentObj->has(RigidBody);
		if(!tc)
			return 0;

		float dir;

		if(!t)
			dir = 1.0;
		else
			dir = t->direction;

		lua_pushnumber(Lstate, tc->getCenter().x);
		lua_pushnumber(Lstate, tc->getCenter().y);
		lua_pushnumber(Lstate, tc->getCenter().z);

		lua_pushnumber(Lstate, tc->getBoundingValues().x);
		lua_pushnumber(Lstate, tc->getBoundingValues().y);
		lua_pushnumber(Lstate, tc->getBoundingValues().z);

		lua_pushnumber(Lstate, tc->getMinVelocity().x);
		lua_pushnumber(Lstate, tc->getMinVelocity().y);
		lua_pushnumber(Lstate, tc->getMinVelocity().z);

		lua_pushnumber(Lstate, tc->getMaxVelocity().x);
		lua_pushnumber(Lstate, tc->getMaxVelocity().y);
		lua_pushnumber(Lstate, tc->getMaxVelocity().z);

		lua_pushnumber(Lstate, tc->getMinTime());

		lua_pushnumber(Lstate, tc->getMaxTime());

		lua_pushnumber(Lstate, rand());

		lua_pushstring(Lstate, CurrentObj->GetName().c_str());

		lua_pushnumber(Lstate, dir);
		
		return 17;
	}	

	//cpp_SetParticleCenter
	//This function imports the transform component of the current object to a Lua script.
	//This allows for an object's position to be known even if it is not the current object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_SetParticleCenter(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 3)
			return 0;

		if(!CurrentObj)
			return 0;

		float x = (float)lua_tonumber(Lstate, 1);
		float y = (float)lua_tonumber(Lstate, 2);
		float z = (float)lua_tonumber(Lstate, 3);

		Particle * tc = CurrentObj->has(Particle);
		if(!tc)
			return 0;

		tc->setCenter(XMFLOAT3(x, y, z));


		return 0;
	}

	//cpp_SetParticleCenter
	//This function imports the transform component of the current object to a Lua script.
	//This allows for an object's position to be known even if it is not the current object.
	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_SetParticleScript(lua_State * Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;

		if(!CurrentObj)
			return 0;

		std::string script = lua_tostring(Lstate, 1);

		Particle * tc = CurrentObj->has(Particle);
		if(!tc)
			return 0;

		tc->setScript(script);


		return 0;
	}	

	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_SetCameraDepth(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;

		float depth = (float)lua_tonumber(Lstate, 1);
		
		float currentdepth = g_GAMEPLAYSYSTEM->GetLevelDepth();
		g_GAMEPLAYSYSTEM->SetLevelDepth(depth);

		if(depth < currentdepth)
			g_GAMEPLAYSYSTEM->ZoomOut();
		else
			g_GAMEPLAYSYSTEM->ZoomIn();

		
		return 0;
	}

	//--------------------------------------------------------------------------------
	int LuaLibrary::cpp_ToggleFree(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 1)
			return 0;

		bool camera = (bool)lua_toboolean(Lstate, 1);

		Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

		c->SetFreeCamera(camera);

		return 0;
	}


	int LuaLibrary::cpp_ButtonPressed(lua_State *Lstate)
	{
		int n = lua_gettop(Lstate);

		if(n != 0)
			return 0;
	

		return 0;
	}
};