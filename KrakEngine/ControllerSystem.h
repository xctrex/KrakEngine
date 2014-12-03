/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: ControllerSystem.h
Purpose: Header for ControllerSystem.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/11/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "isystem.h"

#include <list>

namespace KrakEngine{

	class GameObject;
	class Controller;

	class ControllerSystem :
		public ISystem
	{
	public:
		ControllerSystem();
		~ControllerSystem();

		virtual void HandleMessages(Message* message);

		virtual void Update(float dt);

		virtual bool Initialize();

		virtual bool Shutdown() {return true;};

		void ReturnControlToHat();

		std::list<Controller*> ControllerList;

		GameObject * FindController();
		GameObject * GetHat(){return Hat;}
				
		void ResetHat();

		void SetUnsafeToRelease() {m_unsafe = true;}
		void SetSafeToRelease() {m_unsafe = false;}
	private:
	
		void RemoveControllerComponent(int objectid);
		
		bool SetNewController(GameObject * go);
		bool IsHatCurrentController();
		void SwitchController();

		void ResolveHatCollision(CollisionMessage * CM);

		void ClampHat();
	
		GameObject * Hat;
	
		bool m_unsafe;
	
	};

	extern ControllerSystem* g_CONTROLLERSYSTEM;
};