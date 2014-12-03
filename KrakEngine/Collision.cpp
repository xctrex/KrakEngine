/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or 
disclosure of this file or its contents without the prior written 
consent of DigiPen Institute of Technology is prohibited.
File Name: Collision.cpp
Purpose: Checks collisions between AABB and generates manifold for 
		 resolution.
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: go_ninja
Author: Cristina Pohlenz, p.cristina
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "Collision.h"
#include "Resolution.h"
#include "MathLib.h"

#include "Core.h"

#include "GameObject.h"
#include "Controller.h"

namespace KrakEngine{

	Collisions* g_COLLISIONS = NULL;

	Collisions::Collisions(){
		g_COLLISIONS = this;
		DebugDrawingActive = false;
	}

	Collisions::~Collisions(){}

	void Collisions::checkCollision(RigidBody& it,RigidBody& itB){
		Manifold m;
		if (!(it.mass > 0) && !(itB.mass > 0))
			return;

		if(it.size.z < itB.size.z)
		{
			if (
				it.position.z > itB.position.z + (itB.size.z / 2) ||
				it.position.z < itB.position.z - (itB.size.z / 2)
				)
				return;
		}
		else
		{
			if (
				itB.position.z > it.position.z + (it.size.z / 2) ||
				itB.position.z < it.position.z - (it.size.z / 2)
				)
			return;
		}

		if(it.GetOwner()->GetType() == "car")
		{
			it.prevPos = it.position;
		}
		else if (itB.GetOwner()->GetType() == "car")
		{
			itB.prevPos = itB.position;
		}

		if (AABBtoAABB(it,itB,m)){
			
			CollisionMessage CM(it.GetOwnerID(),itB.GetOwnerID(),it.collision);
			CORE->Broadcast(&CM);

			if(CM.IsCollisionIgnored())
				return;

			g_RESOLUTION->AddCollisionManifold(m);
		
		}
	}

	void Collisions::HandleMessages(Message* message){	
	}

	void Collisions::DetectCollisionFlag(RigidBody& it,RigidBody& itB,Vector3 normal){
		if (normal.y != 0){
			if ( it.position.y > itB.position.y ){
				it.collision |= COLLISION_BOTTOM;
				itB.collision |= COLLISION_TOP;
			}else{
				it.collision |= COLLISION_TOP;
				itB.collision |= COLLISION_BOTTOM;
			}
		}else if (normal.x != 0){
			if ( it.position.x > itB.position.x ){
				it.collision |= COLLISION_LEFT;
				itB.collision |= COLLISION_RIGHT;
			}else{
				it.collision |= COLLISION_RIGHT;
				itB.collision |= COLLISION_LEFT;
			}
		}
	}

	bool Collisions::AABBtoAABB(RigidBody& it,RigidBody& itB, Manifold& m){
		Vector3 n = it.position - itB.position;

		float aExtent = it.size.x / 2.0f;
		float bExtent = itB.size.x / 2.0f;

		float xOverlap = aExtent + bExtent - abs(n.x);

		if (xOverlap > 0){
			float aExtent = it.size.y / 2.0f;
			float bExtent = itB.size.y / 2.0f;

			m.A = &(it);
			m.B = &(itB);

			m.friction = sqrt(pow(it.friction,2) + pow(itB.friction,2.0f));
			m.restitution = std::min<float>(it.restitution,itB.restitution);
		

			float yOverlap = aExtent + bExtent - abs(n.y);

			if (yOverlap > 0){

				if (xOverlap < yOverlap){
					if (n.x < 0){
						m.ContactNormal = Vector3(-1, 0, 0);
					}
					else{
						m.ContactNormal = Vector3(1, 0, 0);
					}
					m.penetration = xOverlap;
					if (m.penetration > 0.5f)
					{
						if ((it.prevPos.z != it.position.z) && (it.mass != 0))
						{
							it.position.z = it.prevPos.z;
							return false;
						}
						if ((itB.prevPos.z != itB.position.z) && (itB.mass != 0))
						{
							itB.position.z = itB.prevPos.z;
							return false;
						}
					}
	
					DetectCollisionFlag(it,itB,m.ContactNormal);
					return true;
				}else{
					if (n.y < 0)
						m.ContactNormal = Vector3(0, -1, 0);
					else
						m.ContactNormal = Vector3(0, 1, 0);
					m.penetration = yOverlap;

					if (m.penetration > 0.5f)
					{
						if ( (it.prevPos.z != it.position.z) && (it.mass != 0))
						{
							it.position.z = it.prevPos.z;
							return false;
						}
						if ((itB.prevPos.z != itB.position.z) && (itB.mass != 0))
						{
							itB.position.z = itB.prevPos.z;
							return false;
						}
					}
	
					DetectCollisionFlag(it,itB,m.ContactNormal);
					return true;
				}
			} //if overlap in Y
		}//if overlap in X
		return false;
	}
}
