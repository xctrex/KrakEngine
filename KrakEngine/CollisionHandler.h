/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: CollisionHandler.h
Purpose: Catches important collisions and resolves them for game play purpose (not for physical resolution).
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Jeff Kitts, j.kitts
Creation date: 2/18/2013
- End Header -----------------------------------------------------*/

#pragma once

#include <map>

namespace KrakEngine
{
	class CollisionMessage;
	class GameObject;

	class CollisionHandler
	{
		public:

		typedef void (CollisionHandler::*CollisionHandlerFunction)();
		typedef std::pair<std::string, std::string> ObjectCombination;
		typedef std::map< ObjectCombination, CollisionHandlerFunction > FunctionMap; 

		CollisionHandler();
		~CollisionHandler();

		void InitializeCollisionFunctionMap();

		void ShrinkOneKillTwo();
		void IgnoreOneStopMoving();
		void IgnoreOneKillTwo();
		void IgnoreCollision();
		void ControllerFeedback();
		void IncreaseButton();
		void CreateSmoke();
		void CreateSwearing();
		void CreateSteam();
		void HeartExplosion();

		void KillOne(GameObject *go, int & countdown);
		//void ShrinkOneKillTwo(GameObject *go, GameObject * go2);
		//void IgnoreOneKillTwo(GameObject * go2, CollisionMessage * CM);
		//void IgnoreOneStopMoving(GameObject *go, CollisionMessage *CM);

		void FindCollision(CollisionMessage * CM);
		void SetupForCollisionResolution(GameObject *go1, GameObject * go2, CollisionMessage * CM, CollisionHandlerFunction ToExecute);

	private:
		FunctionMap CollisionHandlerFunctionMap;
	
		GameObject * m_FirstObject;
		GameObject * m_SecondObject;
		CollisionMessage * m_CurrentMessage;
	};
	
};