/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 2/09/2014
*/

#pragma once
#include "Precompiled.h"
#include "Component.h"
#include "AISystem.h"

namespace KrakEngine{

	class AI : public Component{
	public:
		AI();
		~AI();
		void Initialize();
		void Shutdown();
		void SerializeViaXML(Serializer& stream);
		void SerializeToXML(tinyxml2::XMLElement * element);
	
		void setFlag(int flag){m_flag = flag;}
		int getFlag(){return m_flag;}
		void setStatus(stateStatus status){m_status = status;}
		stateStatus getStatus(){return m_status;}
		void setCurrentState(stateName state){m_CurrentState = state;}
		stateName getCurrentState(){return m_CurrentState;}
		void setPrevState(stateName state){m_PrevState = state;}
		stateName getPrevState(){return m_PrevState;}
		void setCounter(float counter){m_counter = counter;}
		float getCounter(){return m_counter;}
		Vector3 getLeftDestination(){return m_leftDestination;}
		void setLeftDestination(XMFLOAT3 destination){m_leftDestination = destination;}
		Vector3 getRightDestination(){return m_rightDestination;}
		void setRightDestination(XMFLOAT3 destination){m_rightDestination = destination;}
		void setState(state nState){ m_State = nState;}
		state getState(){return m_State;}
		void setPlayerCollide(bool collide){ m_playerCollide = collide;}
		bool getPlayerCollide(){ return m_playerCollide; }

		void Run(GameObject* obj, float dt);

		AI * Next;
		AI * Prev;

	private:
		float m_counter;
		int m_flag;
		stateStatus m_status;
		state m_State;
		stateName m_PrevState;
		stateName m_CurrentState;
		Vector3 m_leftDestination;
		Vector3 m_rightDestination;
		bool m_playerCollide;
		
	};

}