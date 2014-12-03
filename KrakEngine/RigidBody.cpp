/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: RigidBody.cpp
Purpose: Component for Physics
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "RigidBody.h"
#include "Transform.h"
#include "GameObject.h"
#include "Collision.h"

namespace KrakEngine{
	RigidBody::RigidBody() : 
        Component(RigidBodyComp),
        velocity(0.0f, 0.0f, 0.0f),
		acceleration(0.0f, 0.0f, 0.0f),
        speed(0.0f),
        mass(0.0f),
		invMass(0.0f),
		friction(0.0f),
		restitution(0.0f),
        size(0.0f, 0.0f, 0.0f),
        position(0.0f, 0.0f, 0.0f),
		prevPos(0.0f, 0.0f, 0.0f),
        counter(0),
        collision(0),
		direction(0),
		m_noGravity(0)
    {
		g_PHYSICSSYSTEM->Bodies.push_back( this );
	}

	RigidBody::~RigidBody(){
		g_PHYSICSSYSTEM->Bodies.erase(this);
	};

	void RigidBody::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, velocity.x, "velocityX");
		if (velocity.x < 0.001f)
			direction = -1;
		else if (velocity.x > 0.001f)
			direction = 1;
//		else
//			direction = 0;
		InitializeDataMember(stream, velocity.y, "velocityY");
		InitializeDataMember(stream, speed, "speed");
		InitializeDataMember(stream, mass, "mass");
		if (mass == 0.0f)
			invMass = 0.0f;
		else
			invMass = 1.0f/mass;
		InitializeDataMember(stream, friction, "friction");
		InitializeDataMember(stream, restitution, "restitution");
		InitializeDataMember(stream, bodyType, "BodyType");
		InitializeDataMember(stream, size.x, "SizeX");
		if (bodyType == Body::CircleBody){
			size.y = size.x;
		}
		else{
			InitializeDataMember(stream, size.y, "SizeY");
		}
		InitializeDataMember(stream, size.z, "SizeZ");
		InitializeDataMember(stream, m_noGravity, "NoGravity");
		InitializeDataMember(stream, counter, "counter");

	}

	void RigidBody::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, velocity.x, "velocityX");
		if (velocity.x < 0.001f)
			direction = -1;
		else if (velocity.x > 0.001f)
			direction = 1;
//		else
//			direction = 0;
		WriteDataMember(element, velocity.y, "velocityY");
		WriteDataMember(element, speed, "speed");
		WriteDataMember(element, mass, "mass");
		WriteDataMember(element, friction, "friction");
		WriteDataMember(element, restitution, "restitution");
		WriteDataMember(element, bodyType, "BodyType");
		WriteDataMember(element, size.x, "SizeX");
		WriteDataMember(element, size.y, "SizeY");
		WriteDataMember(element, size.z, "SizeZ");
		WriteDataMember(element, m_noGravity, "NoGravity");
		WriteDataMember(element, counter, "counter");
	}

	void RigidBody::Initialize(){
		if (mass > 0)
			invMass = 1.0f/mass;
	}

	void RigidBody::GetPosition(){
        
		transform = GetOwner()->has( Transform );
		prevPos = position;
		position = transform->GetPosition();

		if(velocity.x < 0.01f && velocity.x > -0.01f)
			return;
			
		if (velocity.x < 0.01f)
			direction = -1;
		else if (velocity.x > 0.01f)
			direction = 1;
//		else
//			direction = 0;
	}

	void RigidBody::SetPosition(){
		if(!(velocity.x < 0.01f && velocity.x > -0.01f))
		{
			
		if (velocity.x < 0.01f)
			direction = -1;
		else if (velocity.x > 0.01f)
			direction = 1;
//		else
//			direction = 0;
		
		}

		transform->SetPosition(position);
	}

	void RigidBody::AddForce(Vector3 force){
		forces.push_back(force);
	}

	void RigidBody::Shutdown(){}
	
}