/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: PhysicsSystem.cpp
Purpose: System for Physics, Integrates Rigid Bodies, does collision
		 checks, calls for body resolution.
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninjs
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "PhysicsSystem.h"
#include "Collision.h"
#include "Resolution.h"
#include "RigidBody.h"
#include "MathLib.h"
#include "GameObject.h"

namespace KrakEngine{

	PhysicsSystem * g_PHYSICSSYSTEM = NULL;

	PhysicsSystem::PhysicsSystem() : m_PausePhysics(false){
		g_PHYSICSSYSTEM = this;
		g_COLLISIONS = new Collisions();
		g_RESOLUTION = new Resolution();
		DebugDrawingActive = false;
		m_gravity = Vector3(0, -20.0f, 0);
	}

	PhysicsSystem::~PhysicsSystem(){}

	bool PhysicsSystem::Initialize(){
		return true;
	}

	bool PhysicsSystem::Shutdown(){
		return true;
	}

	void PhysicsSystem::Update(float dt){
		if(m_PausePhysics)
			return;
		//Calculate semi-fixed step
		//time = 1.0f/60.0f;
		//accumulator += dt;

		//deltaTime = std::min<double>(accumulator,time);

			TimeStep(dt);
			//accumulator -=dt;
		
	}

	void PhysicsSystem::TimeStep(float dt){
		GetPositions();

		Integrate(dt);

		BroadPhaseCollision();

		ResolveCollision();

		SetPositions();
	}

	void PhysicsSystem::Integrate( float dt ){
		it = Bodies.begin();
		for (; it != Bodies.end(); ++it){
			it->collision = 0;
			Vector3 oldVelocity = Vector3(0, 0, 0);
			if(it->mass < 0){ //For particles not affected by outside forces
				//update positions
				it->position = it->position + ( it->velocity * dt );
			}
			if (it->mass > 0){
				oldVelocity = it->velocity;

				if(!it->m_noGravity)
					//apply gravity
					it->velocity = it->velocity + m_gravity * dt;


				//HACK!! THIS WILL CHANGE
				if (it->velocity.x < 0)
				{
					it->velocity.x += it->friction;
				}else if (it->velocity.x > 0)
				{
					it->velocity.x -= it->friction;
				}
				// UGLY CODE END

				//update positions
				it->position = it->position + ( ( oldVelocity + it->velocity ) * dt );
			}
		}
	}

	void PhysicsSystem::BroadPhaseCollision(){

		ObjectLinkList<RigidBody>::iterator it = Bodies.begin();
		
		for (; it != Bodies.end(); ++it){	

			ObjectLinkList<RigidBody>::iterator itB = it->Next;
			//ObjectLinkList<RigidBody>::iterator itB = it;
			

			for (; itB != Bodies.end(); ++itB){
				g_COLLISIONS->checkCollision((*it), (*itB));					
			}
		}
	}

	void PhysicsSystem::ResolveCollision(){
		g_RESOLUTION->Resolve();
	}

	void PhysicsSystem::GetPositions(){
		it = Bodies.begin();
        for(;it!=Bodies.end();++it)
		{
			it->GetPosition();
		}
	}
	void PhysicsSystem::SetPositions(){
		it = Bodies.begin();
        for(;it!=Bodies.end();++it)
		{
			it->SetPosition();
		}
	}

	void PhysicsSystem::RemovePhysicsComponent(int id)
	{
		it = Bodies.begin();
        for(;it!=Bodies.end();++it)
        {
			if((*it).GetOwnerID() == id)
			{
				Bodies.erase(it);
				return;	
			}
        }
	}



	void PhysicsSystem::HandleMessages(Message* message){

		if (message->MessageId == MId::Debug)
			DebugDrawingActive = !DebugDrawingActive;

		else if(message->MessageId == MId::ObjectDestroyed)
		{
			ObjectDestroyedMessage * ODM = dynamic_cast<ObjectDestroyedMessage *>(message);
			RemovePhysicsComponent(ODM->GetID());
		}

		else if (message->MessageId == MId::ClearComponentLists)
		{			
			it = Bodies.begin();
			for(;it!=Bodies.end();++it)
			{
				Bodies.erase(it);
			}
		}
		//else if (message->MessageId == MId::TriggerZone)
	
	}

}