/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: Resolution.h
Purpose: Header for PhysicsSystem.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/
#pragma once
#include "Precompiled.h"
#include "Collision.h"
namespace KrakEngine{

	class Resolution{
	
	public:
		Resolution();
		~Resolution();

		void AddCollisionManifold(Manifold m);
		void Resolve();
		void FlushCollisions();

	private:
		void ResolvePosition(Manifold* m);
		void ResolveCollision(Manifold* m);
		void ResetFlags(Manifold* m);

	private:
		std::list<Manifold> collisionsList;
		std::list<Manifold>::iterator it;

	};
	extern Resolution* g_RESOLUTION;
}