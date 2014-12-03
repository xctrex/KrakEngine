/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: Collision.h
Purpose: Header for Collision.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "ObjectLinkedList.h"
#include "RigidBody.h"
#include "PhysicsSystem.h"

namespace KrakEngine{

	struct Manifold{
		Manifold(){}
		RigidBody* A;
		RigidBody* B;
		Vector3 ContactNormal;
		float penetration;
		float restitution;
		float friction;

	};

	class Collisions{

	public:
		Collisions();
		~Collisions();

		void HandleMessages(Message* message);
		void checkCollision(RigidBody& A, RigidBody& B);

	private:

		bool AABBtoAABB(RigidBody& it,RigidBody& itB, Manifold& m);
		void DetectCollisionFlag(RigidBody& it,RigidBody& itB,Vector3 normal);

	public:
		ObjectLinkList<RigidBody> Bodies;
		bool DebugDrawingActive;

	};

	class Body{
	
	public:
	
		enum bodyTypes
		{
			CircleBody,
			RecBody,
			LineBody,
		};

		bodyTypes bType;
		RigidBody* obj;

		Body(bodyTypes type) : bType(type) {};

		virtual void Draw() = 0;
	};

	extern Collisions* g_COLLISIONS;
}