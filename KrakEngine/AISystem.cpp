/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 2/09/2014
*/

#include "Precompiled.h"
#include "AISystem.h"
#include "AIComp.h"
#include "GamePlaySystem.h"

#include <iostream>

namespace KrakEngine{

	AISystem* g_AISYSTEM = NULL;

	AISystem::AISystem() : m_Paused(false)
	{
		g_AISYSTEM = this;
	}

	AISystem::AISystem(WindowSystem* window) : m_Paused(false)
	{
		m_window = window;
		g_AISYSTEM = this;
	}

	AISystem::AISystem(const AISystem&) : m_Paused(false)
	{
	}

	AISystem::~AISystem()
	{
	}

	bool AISystem::Initialize()
	{
		return true;
	}

	void AISystem::Update(float dt){
		if(m_Paused) return;

		ObjectLinkList<AI>::iterator it = m_AIObjects.begin();
		for(;it !=m_AIObjects.end();++it){
			it->Run((*it).GetOwner(), dt);
		}
	}

	bool AISystem::Shutdown()
	{
		return true;
	}

	void AISystem::HandleMessages(Message* message){
		
		if(message->MessageId == MId::ObjectDestroyed){
			ObjectDestroyedMessage * ODM = dynamic_cast<ObjectDestroyedMessage *>(message);
			RemoveAIComponent(ODM->GetID());
		}else if(message->MessageId == MId::ClearComponentLists){

			ObjectLinkList<AI>::iterator it = m_AIObjects.begin();
			for(;it !=m_AIObjects.end();++it){
				m_AIObjects.erase(it);
			}
		}else if(message->MessageId == MId::Collision){
			CollisionMessage * ODM = dynamic_cast<CollisionMessage *>(message);
			ObjectLinkList<AI>::iterator it = m_AIObjects.begin();
			for(;it !=m_AIObjects.end();++it){
				if((*it).GetOwnerID() == ODM->GetObjectID() || (*it).GetOwnerID() == ODM->GetColliderID() ){
					(*it).setFlag(ODM->GetCollisionFlag());
					Collider = g_FACTORY->GetObjList().FindByID(ODM->GetObjectID());
					Player = g_FACTORY->GetObjList().FindByID(ODM->GetColliderID());
					if(Collider->GetType() == "burglar" && Player->GetName() == "Player"){
						(*it).setPlayerCollide(true);
					}else if(Player->GetType() == "burglar" && Collider->GetName() == "Player"){
						(*it).setPlayerCollide(true);
					}
				}
			}
		}
	}

	void AISystem::RemoveAIComponent(int id){

		ObjectLinkList<AI>::iterator it = m_AIObjects.begin();
        for(;it!=m_AIObjects.end();++it)
        {
			if((*it).GetOwnerID() == id)
			{
				m_AIObjects.erase(it);
				return;	
			}
        }
	}

	//States
	void AISystem::Wait(GameObject *obj, float dt){

		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);

		if(objAI->getStatus() == onEntry){
			objAI->setCounter(5.0f);
			objAI->setStatus(onGoing);
			objAI->setCurrentState(s_Wait);
		}

		if(objAI->getStatus() == onGoing){
			if(objCon->IsCurrentController()){
				objAI->setStatus(onExit);
			}else if(objAI->getCounter() <= 0){
				objAI->setStatus(onExit);
			}else{
				objAI->setCounter(objAI->getCounter() - dt);
			}
		}

		if(objAI->getStatus() == onExit){
			if(objCon->IsCurrentController()){
				objAI->setState(&AISystem::Controlled);
			}else if(objAI->getPrevState() == s_WanderLeft){
				objAI->setState(&AISystem::WanderRight);
			}else if(objAI->getPrevState() == s_WanderRight){
				objAI->setState(&AISystem::WanderLeft);
			}else if(objAI->getPrevState() == s_MoveLeft){
				objAI->setState(&AISystem::MoveRight);
			}else if(objAI->getPrevState() == s_MoveRight){
				objAI->setState(&AISystem::MoveLeft);
			}
			objAI->setPrevState(s_Wait);
			objAI->setStatus(onEntry);
		}
	}

	void AISystem::WanderLeft(GameObject *obj, float dt){

		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);

		if(objAI->getStatus()== onEntry){
			//Change animation to walk
			objAI->setStatus(onGoing);
			objPhysics->velocity.x = -objPhysics->speed;
			objAI->setCurrentState(s_WanderLeft);
		}

		if(objAI->getStatus() == onGoing){
			objPhysics->velocity.x = -objPhysics->speed;
			if(objCon->IsCurrentController()){
				objAI->setStatus(onExit);
			}else if(objAI->getFlag() & COLLISION_LEFT){
				objPhysics->velocity.x = 0;
				objAI->setStatus(onExit);
			}
		}

		if(objAI->getStatus() == onExit){
			if(objCon->IsCurrentController()){
				objAI->setState(&AISystem::Controlled);
			}else{
				objAI->setState(&AISystem::Wait);
			}
			objAI->setPrevState(s_WanderLeft);
			objAI->setStatus(onEntry);
		}
	}

	void AISystem::WanderRight(GameObject *obj, float dt){

		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);

		if(objAI->getStatus()== onEntry){
			//Change animation to walk
			objAI->setStatus(onGoing);
			objPhysics->velocity.x = objPhysics->speed;
			objAI->setCurrentState(s_WanderRight);
		}

		if(objAI->getStatus() == onGoing){
			objPhysics->velocity.x = objPhysics->speed;
			if(objCon->IsCurrentController()){
				objAI->setStatus(onExit);
			}else if(objAI->getFlag() & COLLISION_RIGHT){
				objPhysics->velocity.x = 0;
				objAI->setStatus(onExit);
			}
		}

		if(objAI->getStatus() == onExit){
			if(objCon->IsCurrentController()){
				objAI->setState(&AISystem::Controlled);
			}else{
				objAI->setState(&AISystem::Wait);
			}
			objAI->setPrevState(s_WanderRight);
			objAI->setStatus(onEntry);
		}
	}

	void AISystem::MoveLeft(GameObject *obj, float dt){

		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);

		if(objAI->getStatus()== onEntry){
			//Change animation to walk
			objAI->setStatus(onGoing);
			objPhysics->velocity.x = -objPhysics->speed;
			objAI->setCurrentState(s_MoveLeft);
		}

		if(objAI->getStatus() == onGoing){
			objPhysics->velocity.x = -objPhysics->speed;
			if(objCon->IsCurrentController()){
				objAI->setStatus(onExit);
			}else if(objAI->getLeftDestination().x >= objTransform->GetPosition().x){
				objPhysics->velocity.x = 0;
				objAI->setStatus(onExit);
			}
		}

		if(objAI->getStatus() == onExit){
			if(objCon->IsCurrentController()){
				objAI->setState(&AISystem::Controlled);
			}else{
				objAI->setState(&AISystem::Wait);
			}
			objAI->setPrevState(s_MoveLeft);
			objAI->setStatus(onEntry);
		}
	}

	void AISystem::MoveRight(GameObject *obj, float dt){

		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);

		if(objAI->getStatus()== onEntry){
			//Change animation to walk
			objAI->setStatus(onGoing);
			objPhysics->velocity.x = objPhysics->speed;
			objAI->setCurrentState(s_MoveRight);
		}

		if(objAI->getStatus() == onGoing){
			objPhysics->velocity.x = objPhysics->speed;
			if(objCon->IsCurrentController()){
				objAI->setStatus(onExit);
			}else if(objAI->getRightDestination().x <= objTransform->GetPosition().x){
				objPhysics->velocity.x = 0;
				objAI->setStatus(onExit);
			}
		}

		if(objAI->getStatus() == onExit){
			if(objCon->IsCurrentController()){
				objAI->setState(&AISystem::Controlled);
			}else{
				objAI->setState(&AISystem::Wait);
			}
			objAI->setPrevState(s_MoveRight);
			objAI->setStatus(onEntry);
		}
	}

	void AISystem::Controlled(GameObject *obj, float dt){
		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);

		if(objAI->getStatus()== onEntry){
			objAI->setStatus(onGoing);
			objAI->setCurrentState(s_Controlled);
		}

		if(objAI->getStatus() == onGoing){
			if(!objCon->IsCurrentController()){
				objAI->setStatus(onExit);
			}
		}

		if(objAI->getStatus() == onExit){
			objAI->setState(serializeState(objAI->getPrevState()));
			objAI->setPrevState(s_Controlled);
			objAI->setStatus(onEntry);
		}
	}

	void AISystem::NoState(GameObject *obj, float dt){
	}

	void AISystem::Car(GameObject *obj, float dt){
		objTransform = obj->has(Transform);
			XMFLOAT3 posvec = objTransform->GetPosition();
			posvec.x -= 20.0f * dt;
			objTransform->SetPosition(posvec);
	}

	void AISystem::Burglar(GameObject *obj, float dt){
		objPhysics = obj->has(RigidBody);
		objTransform = obj->has(Transform);
		objAI = obj->has(AI);
		objCon = obj->has(Controller);
		Player = g_CONTROLLERSYSTEM->FindController();
		if(Player){
			playerControl = Player->has(Controller);
			playerPhy = Player->has(RigidBody);
		}

		if(objAI->getStatus()== onEntry){
			objAI->setStatus(onGoing);
			objAI->setCurrentState(s_Burglar);
			objPhysics->position.x = objAI->getRightDestination().x;
			objPhysics->SetPosition();
			objPhysics->velocity = Vector3(0,0,0);
		}

		if(objAI->getStatus() == onGoing){
			if(!PoliceInArea(objAI->getLeftDestination(), objAI->getRightDestination()) && playerControl->GetOwner()->GetType() != "policeman"){
				if(playerPhy->position.x >= objAI->getLeftDestination().x){
					objPhysics->velocity.x = -objPhysics->speed;
				}
				if(playerPhy->velocity.y > 0 && objPhysics->velocity.y == 0){
					objPhysics->velocity.y = 10.0f;
				}
				if(playerPhy->position.z != objPhysics->position.z){
					objPhysics->position.z = playerPhy->position.z;
					objPhysics->SetPosition();
				}
				if(objAI->getPlayerCollide()){
					playerPhy->position = objPhysics->position;
					playerPhy->SetPosition();
					playerPhy->velocity = objPhysics->velocity;
					objAI->setStatus(onExit);	
				}
				if(objPhysics->position.x <= objAI->getLeftDestination().x){
					objAI->setStatus(onExit);
				}
			}
		}

		if(objAI->getStatus() == onExit){
			if(objPhysics->position.x <= objAI->getLeftDestination().x){
				objAI->setStatus(onEntry);
				objAI->setPlayerCollide(false);
				playerPhy->position = objPhysics->position;
				playerPhy->position.x = objPhysics->position.x - 3;
				playerPhy->SetPosition();
			}else{
				playerPhy->position = objPhysics->position;
				playerPhy->position.z = objPhysics->position.z - 0.01f;
				playerPhy->SetPosition();
				playerPhy->velocity = objPhysics->velocity;
			}

		}
	}

	bool AISystem::PoliceInArea(Vector3 left, Vector3 right){
		ObjectLinkList<AI>::iterator it = m_AIObjects.begin();
		Transform* trans;
		for(;it !=m_AIObjects.end();++it){
			if(it->GetOwner()->GetType() == "policeman"){
				trans = it->GetOwner()->has(Transform);
				if(trans){
					if(trans->GetPosition().x > left.x && trans->GetPosition().x < right.x){
						return true;
					}
				}
			}
		}
		return false;
	}

	state AISystem::serializeState(std::string nState){
		if(nState == "Wait"){
			return &AISystem::Wait;
		}else if(nState == "WanderLeft"){
			return &AISystem::WanderLeft;
		}else if(nState == "WanderRight"){
			return &AISystem::WanderRight;
		}else if(nState == "MoveLeft"){
			return &AISystem::MoveLeft;
		}else if(nState == "MoveRight"){
			return &AISystem::MoveRight;
		}else if(nState == "Burglar"){
			return &AISystem::Burglar;
		}else if(nState == "Car"){
			return &AISystem::Car;
		}else{
			return &AISystem::NoState;
		}
	}

	state AISystem::serializeState(stateName nState){
		if(nState == s_Wait){
			return &AISystem::Wait;
		}else if(nState == s_WanderLeft){
			return &AISystem::WanderLeft;
		}else if(nState == s_WanderRight){
			return &AISystem::WanderRight;
		}else if(nState == s_MoveLeft){
			return &AISystem::MoveLeft;
		}else if(nState == s_MoveRight){
			return &AISystem::MoveRight;
		}else if(nState == s_Burglar){
			return &AISystem::Burglar;
		}else if(nState == s_Car){
			return &AISystem::Car;
		}
		else{
			return &AISystem::NoState;
		}
	}

}