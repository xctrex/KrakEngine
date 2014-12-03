/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: PhysicsSystem.h
Purpose: Header for PhysicsSystem.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "ISystem.h"
#include "ObjectLinkedList.h"

namespace KrakEngine{

	class RigidBody;

	class PhysicsSystem : public ISystem{
	
		public:
		PhysicsSystem();
		~PhysicsSystem();

		bool Initialize();
		bool Shutdown();
		void Update(float dt);
		void HandleMessages(Message* message);
		void PausePhysics() { m_PausePhysics = true; }
		void UnpausePhysics() { m_PausePhysics = false; }

	private:
		void TimeStep (float dt);
		void Integrate(float dt);
		void GetPositions();
		void SetPositions();
		void RemovePhysicsComponent(int id);
		void BroadPhaseCollision();
		void ResolveCollision();

	public:
		ObjectLinkList<RigidBody> Bodies;
		bool DebugDrawingActive;

	private:
		bool m_PausePhysics;
		XMFLOAT3 m_gravity;
		ObjectLinkList<RigidBody>::iterator it;
		double time;
		double accumulator;
		double deltaTime;

	};

	extern PhysicsSystem* g_PHYSICSSYSTEM;

}