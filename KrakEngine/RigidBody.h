/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: RigidBody.h
Purpose: Header for PhysicsComp.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/
#pragma once
#include "Precompiled.h"
#include "Component.h"
#include "PhysicsSystem.h"

namespace KrakEngine{

	typedef XMFLOAT2   Vector2;
	typedef XMFLOAT3   Vector3;
	typedef XMFLOAT4X4 Matrix4;

	class Transform;

	class RigidBody : public Component{
	public:
		friend class PhysicsSystem;

		RigidBody();
		~RigidBody();
		void Initialize();
		void Shutdown();
		void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);
		void GetPosition();
		void SetPosition();
		void AddForce(Vector3 force);
		Vector3 GetSize() { return size; };
		void SetSize(const Vector3 & newsize)
		{
			size = newsize;
		}

		void SetSize(float x, float y, float z)
		{
			size.x = x;
			size.y = y;
			size.z = z;
		}

		RigidBody * Next;
		RigidBody * Prev;

		Vector3 velocity;
		Vector3 acceleration;
		
		std::list<Vector3> forces;
		float speed;
		float mass;
		float invMass;
		float friction;
		float restitution;
		float direction;
		bool m_noGravity;

		int counter;
		int bodyType;

		Vector3 size;
		Vector3 position;
		Vector3 prevPos;
		int collision;

	private:
		Transform* transform;
	};
}