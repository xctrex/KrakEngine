/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: CollisionHandler.cpp
Purpose: Catches important collisions and resolves them for game play purpose (not for physical resolution).
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Jeff Kitts, j.kitts
Creation date: 2/18/2013
- End Header -----------------------------------------------------*/

#include "CollisionHandler.h"
#include "Message.h"
#include "Factory.h"
#include "GameObject.h"
#include "Model.h"
#include "Controller.h"
#include "AIComp.h"
#include "GamePlaySystem.h"
#include "Particle.h"
#include "Timer.h"
#include "InputSystem.h"

namespace KrakEngine
{
	CollisionHandler::CollisionHandler() 
	{
		InitializeCollisionFunctionMap();
	}
	CollisionHandler::~CollisionHandler() {}

	//InitializeCollisionFunctionMap
	//Initializes the pairs of object types and functions that the collision handler will use in order to resolve the collision.
	//New pairs needed to be added here.
	void CollisionHandler::InitializeCollisionFunctionMap()
	{
		CollisionHandlerFunctionMap.emplace(std::make_pair("fire","water"), &CollisionHandler::ShrinkOneKillTwo );
		CollisionHandlerFunctionMap.emplace(std::make_pair("fireman","fire"), &CollisionHandler::IgnoreOneStopMoving );
		//CollisionHandlerFunctionMap.emplace(std::make_pair("floor","water"), &CollisionHandler::IgnoreOneKillTwo );
		//CollisionHandlerFunctionMap.emplace(std::make_pair("floor","mimebox"), &CollisionHandler::IgnoreOneKillTwo );
		//CollisionHandlerFunctionMap.emplace(std::make_pair("mimebox","mimebox"), &CollisionHandler::IgnoreOneKillTwo );
		CollisionHandlerFunctionMap.emplace(std::make_pair("floor","train"), &CollisionHandler::IgnoreCollision );
		//CollisionHandlerFunctionMap.emplace(std::make_pair("fireman","mimebox"), &CollisionHandler::IgnoreOneKillTwo );
		//CollisionHandlerFunctionMap.emplace(std::make_pair("mime","mimebox"), &CollisionHandler::IgnoreOneKillTwo );
		//CollisionHandlerFunctionMap.emplace(std::make_pair("player","mimebox"), &CollisionHandler::IgnoreOneKillTwo );

		//CollisionHandlerFunctionMap.emplace(std::make_pair("mimebox","floor"), &CollisionHandler::IgnoreCollision );		
		CollisionHandlerFunctionMap.emplace(std::make_pair("mime","mimebox"), &CollisionHandler::IgnoreOneKillTwo );	
			
		CollisionHandlerFunctionMap.emplace(std::make_pair("fireman","mimebox"), &CollisionHandler::IgnoreOneKillTwo );	
		//CollisionHandlerFunctionMap.emplace(std::make_pair("mimebox","fireman"), &CollisionHandler::IgnoreCollision );		

		CollisionHandlerFunctionMap.emplace(std::make_pair("carkill","car"), &CollisionHandler::IgnoreOneKillTwo );
		CollisionHandlerFunctionMap.emplace(std::make_pair("car","floor"), &CollisionHandler::IgnoreCollision );

		CollisionHandlerFunctionMap.emplace(std::make_pair("floor","particle"), &CollisionHandler::IgnoreCollision );		
		CollisionHandlerFunctionMap.emplace(std::make_pair("particle","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("player","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("fireman","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("mime","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("femalehat","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("burglar","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("policeman","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("water","particle"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("floor","water"), &CollisionHandler::IgnoreCollision );		
		CollisionHandlerFunctionMap.emplace(std::make_pair("water","water"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("player","water"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("fireman","water"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("fireman","steammachine"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("mime","water"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("burglar","water"), &CollisionHandler::IgnoreCollision );
		CollisionHandlerFunctionMap.emplace(std::make_pair("policeman","water"), &CollisionHandler::IgnoreCollision );	
		CollisionHandlerFunctionMap.emplace(std::make_pair("player","car"), &CollisionHandler::CreateSwearing );
		CollisionHandlerFunctionMap.emplace(std::make_pair("player","fire"), &CollisionHandler::CreateSmoke );
		CollisionHandlerFunctionMap.emplace(std::make_pair("mime","fire"), &CollisionHandler::ControllerFeedback );


		CollisionHandlerFunctionMap.emplace(std::make_pair("steammachine","water"), &CollisionHandler::CreateSteam );
	
		
		CollisionHandlerFunctionMap.emplace(std::make_pair("player","femalehat"), &CollisionHandler::HeartExplosion );
		
		CollisionHandlerFunctionMap.emplace(std::make_pair("triggerbutton", "mime"), &CollisionHandler::IncreaseButton );
		CollisionHandlerFunctionMap.emplace(std::make_pair("triggerbutton", "mimebox"), &CollisionHandler::IncreaseButton );
		CollisionHandlerFunctionMap.emplace(std::make_pair("triggerbutton", "player"), &CollisionHandler::IgnoreCollision );
	}
	
	//FindCollision
	//Finds whether a collision is defined between the pair of objects given, if so it will
	//execute the collision.

	//Note: this function does not care about order, so if a "fire" interacts with a "fireman", it should find the
	//entry for "fireman", "fire".
	void CollisionHandler::FindCollision(CollisionMessage * CM)
	{
		GameObject * go1 = g_FACTORY->GetObjList().FindByID(CM->GetObjectID());
		GameObject * go2 = g_FACTORY->GetObjList().FindByID(CM->GetColliderID());
		
		ObjectCombination OneTwo = std::make_pair(go1->GetType(), go2->GetType());
		ObjectCombination TwoOne = std::make_pair(go2->GetType(), go1->GetType());

		FunctionMap::iterator it = CollisionHandlerFunctionMap.find(OneTwo);
		
		if(it == CollisionHandlerFunctionMap.end())
		{
			it = CollisionHandlerFunctionMap.find(TwoOne);
			if(it == CollisionHandlerFunctionMap.end())
				return;	
			else
			{
				SetupForCollisionResolution(go2, go1, CM, it->second);
			}
		}
		else
			SetupForCollisionResolution(go1, go2, CM, it->second);
	}

	//SetupForCollisionResolution
	//Puts necessary data onto the CollisionHandler object.

	//This is because all the functions need to have the same signature, so I did void Function ();
	//If new variables need to be added, they'll need to be added here and loaded into the collision handling "pipeline."
	void CollisionHandler::SetupForCollisionResolution(GameObject * go1, GameObject * go2, CollisionMessage * CM, CollisionHandlerFunction toExecute)
	{
		m_FirstObject = go1;
		m_SecondObject = go2;
		m_CurrentMessage = CM;

		(this->*(toExecute))();
	}

	//COLLISION HANDLING FUNCTIONS
	//
	//
	//The following functions are functions that act upon the game objects and handle any gameplay related issues that a collision may enact.
	//
	//One refers to the first object, while two refers to the second object. The first and second are the position the pair in the InitializeCollisionFunctionMap function

	//ShrinkOneKillTwo
	//Reduces the size of the first object, while destroying the second object.
	
	//For example: A fire being sprayed by water.
	void CollisionHandler::ShrinkOneKillTwo()
	{
		RigidBody * rb = m_FirstObject->has(RigidBody);
		if (rb->counter == 0)
			m_FirstObject->Kill();
		else
			rb->counter--;

		Timer* t = m_SecondObject->has(Timer);
		if(t)
			t->Execute();
		m_SecondObject->Kill();

		/*
		Particle * pc = m_FirstObject->has(Particle);
		
		if(pc->getMinParticles() == pc->getMaxParticles())
			m_FirstObject->Kill();
		else
			pc->decreaseMaxParticle(1);

		Timer* t = m_SecondObject->has(Timer);
		if(t)
			t->Execute();
		m_SecondObject->Kill();
		*/
	}

	//IgnoreOneKillTwo
	//Causes the first object to ignore collisions, while killing the second object.
	
	//For example: water drops onto a wall and is destroyed, and the wall is not affected by water (may not be necessary to ignore)
	void CollisionHandler::IgnoreOneKillTwo()
	{
		m_SecondObject->Kill();
		m_CurrentMessage->IgnoreCollision();
	}

	void CollisionHandler::IgnoreCollision()
	{
		m_CurrentMessage->IgnoreCollision();
	}

	//IgnoreOneStopMoving
	//Causes the first one to ignore collision and stop moving.

	//For example: fireman stops moving with fire if controlled by AI, and if not, it just simply ignores the collision.
	void CollisionHandler::IgnoreOneStopMoving()
	{
		Controller * c = m_FirstObject->has(Controller);
		if(!c->IsCurrentController())
		{
			AI * aic = m_FirstObject->has(AI);
			if(aic)
			{
				aic->setState(&AISystem::Wait);
			}
		}

		g_CONTROLLERSYSTEM->SetUnsafeToRelease();

		m_CurrentMessage->IgnoreCollision();
	}

	//ControllerFeedback
	//Sets the vibration of the controller

	//For example: If the player is colliding with fire, the controller rumbles
	void CollisionHandler::ControllerFeedback()
	{
		g_INPUTSYSTEM->SetVibration(16000);
	}

	void CollisionHandler::CreateSmoke()
	{
		g_INPUTSYSTEM->SetVibration(16000);

		LuaInterpreter Li;	
		Li.LoadScript("smoke");	
	}

	void CollisionHandler::CreateSteam()
	{
		RigidBody * rb = m_FirstObject->has(RigidBody);
		if (rb->counter == 0)
			m_FirstObject->Kill();
		else
			rb->counter--;

		LuaInterpreter Li;	
		Li.SetCurrentObj(m_FirstObject);
		Li.LoadScript("steam");	
	}


	void CollisionHandler::CreateSwearing()
	{
		g_INPUTSYSTEM->SetVibration(16000);

		RigidBody * rbFirst = m_FirstObject->has(RigidBody);
		RigidBody * rbSecond = m_SecondObject->has(RigidBody);

		if (rbFirst && rbFirst->velocity.x <= 0.1f && rbFirst->velocity.x >= -0.1f)
		{
			rbFirst->velocity.x = rbSecond->velocity.x;
		}

		LuaInterpreter Li;

		Li.SetCurrentObj(m_SecondObject);
		Li.LoadScript("swear");		
	}

	void CollisionHandler::HeartExplosion()
	{
		LuaInterpreter Li;

		Li.SetCurrentObj(m_FirstObject);
		Li.LoadScript("heart");		

		Li.SetCurrentObj(m_SecondObject);
		Li.LoadScript("heart");		

		CORE->SetNextState(GameStates::Victory);
	}
	
	//ButtonPressed
	//alerts that a button object has collided with an appropriate object.
	//this only connects to one button in the game
	void CollisionHandler::IncreaseButton()
	{
		g_GAMEPLAYSYSTEM->GetMechanics().ButtonPressed(m_FirstObject->GetID());
		m_CurrentMessage->IgnoreCollision();
	}

}
