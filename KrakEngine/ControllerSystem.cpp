/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: ControllerSystem.cpp
Purpose: Handles the control of the player
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/11/2014
- End Header -----------------------------------------------------*/

#include "ControllerSystem.h"
#include "GameObject.h"
#include "Controller.h"
#include "RigidBody.h"
#include "GamePlaySystem.h"
#include "Transform.h"
#include "Core.h"
#include "Audio.h"

namespace KrakEngine{

	ControllerSystem* g_CONTROLLERSYSTEM = NULL;

	ControllerSystem::ControllerSystem() :
		m_unsafe(false)
	{
		g_CONTROLLERSYSTEM = this;
	}

	ControllerSystem::~ControllerSystem()
	{
	}


	bool ControllerSystem::Initialize() {
		Hat = FindController();
		if (Hat)
			return true;
		return false;
	};


	void ControllerSystem::Update(float dt)
	{
		if(CORE->GetCurrentState() != GameStates::InGame) return;

		if(!IsHatCurrentController())
			ClampHat();

		m_unsafe = false;
	}

	void ControllerSystem::HandleMessages(Message* message)
	{
		if(message->MessageId == MId::ObjectDestroyed){
			ObjectDestroyedMessage * ODM = dynamic_cast<ObjectDestroyedMessage *>(message);
			RemoveControllerComponent(ODM->GetID());
		}
		else if (message->MessageId == MId::ClearComponentLists){

				ControllerList.clear();
		}

		else if (message->MessageId == MId::Collision)
		{
			if(IsHatCurrentController())
			{
				CollisionMessage * CM = dynamic_cast<CollisionMessage *>(message);
				if(CM->GetColliderID() == Hat->GetID() || CM->GetObjectID() == Hat->GetID())
					ResolveHatCollision(CM);
			}
		}
	}

	//RemoveControllerComponent
	//Removes the controller of object objectid from the ControllerList.
	void ControllerSystem::RemoveControllerComponent(int objectid)
	{
		std::list<Controller*>::iterator it = ControllerList.begin();
		for(;it!=ControllerList.end();++it)
		{
			if((*it)->GetOwnerID() == objectid)
			{
				ControllerList.erase(it);
				return;	
			}
		}
	}

	//SetNewController
	//if go is a valid object that can recieve control, go will become the controlled object
	//as Player will now point to it.
	bool ControllerSystem::SetNewController(GameObject * go)
	{
		if(!go)
			return false;

		Controller * c = go->has(Controller);
		if(!c)
			return false;

		c->SetPlayerStatus(true);

		SwitchController();

		return true;
	}

	//Switch Controller
	//Turns off control of the current object, and kills its movement, then calls FindController to replace control.
	void ControllerSystem::SwitchController()
	{
		if (IsHatCurrentController()){
			Audio * a = Hat->has(Audio);
			if(a)
				a->playAction1();
		}else
		{
			Audio * a = Hat->has(Audio);
			if(a)
				a->playAction2();
		}
		
		Controller * c = g_GAMEPLAYSYSTEM->Player->has(Controller);
		c->SetPlayerStatus(false);

		RigidBody * pc = g_GAMEPLAYSYSTEM->Player->has(RigidBody);
		pc->velocity.x = 0;
		pc->velocity.y = 0;
		pc->velocity.z = 0;

		g_GAMEPLAYSYSTEM->Player = FindController();
	}

	//FindController
	//Looks through each object and identifies the (first) one labeled as the current controller.
	GameObject * ControllerSystem::FindController()
	{
		std::list<Controller*>::iterator it = ControllerList.begin();
		for(;it!=ControllerList.end();++it)
		{
			if((*it)->IsCurrentController())
			{
				return (*it)->GetOwner();
			}
		}

		return NULL;
	}

	bool ControllerSystem::IsHatCurrentController()
	{
		if(!Hat)
			return false;

		Controller * c = Hat->has(Controller);
		if(c)
			return c->IsCurrentController();
		return false;
	}
	
	//Checks if the hat has collided with an object, and whether or not it should
	//relinquish control to the newly collided object.
	void ControllerSystem::ResolveHatCollision(CollisionMessage * CM)
	{
		int notHat = 0;
		if(CM->GetColliderID() == Hat->GetID())
		{
			notHat = CM->GetObjectID();
			if(!(CM->GetCollisionFlag() & 1 << 2))
				return;
		}
		else
		{
			notHat = CM->GetColliderID();
			if(!(CM->GetCollisionFlag() & 1 << 3))
				return;
		}

		GameObject * NotHatObj = g_FACTORY->GetObjList().FindByID(notHat);

		SetNewController(NotHatObj);		
	}

	//Gives control back to the hat (basically releasing from the object)
	void ControllerSystem::ReturnControlToHat()
	{
		if(m_unsafe)
			return;
		
		if(!Hat)
			ResetHat();

		if(!IsHatCurrentController())
		{
			GameObject * go = FindController();
			Audio * ac = go->has(Audio);

			if(ac)
				ac->playAction2();

			SetNewController(Hat);
			RigidBody *pc = Hat->has(RigidBody)
				if(pc)
				{
					pc->velocity.y = 10.0f;
				}
		}
	}

	//Puts the hat atop the object that now has control.
	void ControllerSystem::ClampHat()
	{
		if(!g_GAMEPLAYSYSTEM->Player)
			return;

		//Controller * cc = Hat->has(Controller)
		//{
		//	if(!cc->IsCurrentController())
		//		return;
		//}

		Transform *tcNotHat = g_GAMEPLAYSYSTEM->Player->has(Transform);

		Transform *tc = Hat->has(Transform);
		if(tc)
		{
			XMFLOAT3 pos;
			if(tcNotHat)
			{
				pos = tcNotHat->GetPosition();

				RigidBody *notHatPhC = g_GAMEPLAYSYSTEM->Player->has(RigidBody);
				if(notHatPhC)
				{
					RigidBody *Hatpc = Hat->has(RigidBody);
					if(Hatpc)
					{
						Vector3 HatSize = Hatpc->GetSize();
						pos.y += HatSize.y/2;
					}	

					Vector3 NotHatSize = notHatPhC->GetSize();
					pos.y += NotHatSize.y/2;
				}				
			}
			else
				pos = tc->GetPosition(); 

			tc->SetPosition(pos);
		}
	}
	
	//Finds the hat
	void ControllerSystem::ResetHat()
	{
		Hat = g_FACTORY->GetObjList().FindByName("Player");
		if(!Hat)
			Hat = g_FACTORY->GetObjList().FindByName("player");
	};

}