/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 2/09/2014
*/

#pragma once
#include "ISystem.h"
#include "WindowSystem.h"
#include "ObjectLinkedList.h"
#include "GameObject.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Controller.h"


namespace KrakEngine{

	class AISystem;

typedef void (AISystem::*state)(GameObject*, float); 

	class AI;

	//Collision flags
	#define	COLLISION_LEFT		0x00000001	//0001
	#define	COLLISION_RIGHT		0x00000002	//0010
	#define	COLLISION_TOP		0x00000004	//0100
	#define	COLLISION_BOTTOM	0x00000008	//1000

	enum stateStatus{
		onEntry = 0,
		onGoing,
		onExit
	};

	enum stateName{
		s_Wait = 0,
		s_WanderLeft,
		s_WanderRight,
		s_MoveLeft,
		s_MoveRight,
		s_Controlled,
		s_NoState,
		s_Burglar,
		s_Car,
	};

	class AISystem : public ISystem{

	public:
		AISystem();
		AISystem(WindowSystem* window);
		AISystem(const AISystem&);
		~AISystem();

		bool Initialize();
		void Update(float dt);
		bool Shutdown();
		void HandleMessages(Message* message);

		state serializeState(std::string nState);
		state serializeState(stateName nState);

		void RemoveAIComponent(int id);

		bool PoliceInArea(Vector3 left, Vector3 right);
		
		void PauseAI() {m_Paused = true;};
		void UnpauseAI() {m_Paused = false;};

		//States
		void Wait(GameObject *obj, float dt);
		void WanderLeft(GameObject *obj, float dt);
		void WanderRight(GameObject *obj, float dt);
		void MoveLeft(GameObject *obj, float dt);
		void MoveRight(GameObject *obj, float dt);
		void Controlled(GameObject *obj, float dt);
		void NoState(GameObject *obj, float dt);
		void Burglar(GameObject *obj, float dt);
		void Car(GameObject *obj, float dt);
		void Train(GameObject *obj, float dt);

	protected:
		//Pointers
		RigidBody* objPhysics;
		Transform* objTransform;
		AI* objAI;
		Controller* objCon;
		GameObject * Player;
		GameObject* Collider;
		Controller* playerControl;
		RigidBody* playerPhy;

	private:
		bool m_Paused;

	public:
		WindowSystem* m_window;
		ObjectLinkList<AI> m_AIObjects;
	};
	extern AISystem* g_AISYSTEM;

}
