/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 2/09/2014
*/

#include "AIComp.h"

#include <iostream>

namespace KrakEngine{


	AI::AI(): Component(AIComp), m_status(onEntry), m_flag(0), m_leftDestination(0, 0, 0), m_rightDestination(0, 0, 0), m_counter(0), m_PrevState(s_NoState), m_CurrentState(s_NoState), m_playerCollide(false) {
		m_State = &AISystem::NoState;
		g_AISYSTEM->m_AIObjects.push_back(this);
	}

	AI::~AI(){}

	void AI::Run(GameObject* obj, float dt){
		(g_AISYSTEM->*m_State) (obj, dt);
	}

	void AI::SerializeViaXML(Serializer& stream){
		std::string temp;
		InitializeDataMember(stream, temp, "state");
		InitializeDataMember(stream, m_leftDestination.x, "leftDestinationX");
		InitializeDataMember(stream, m_leftDestination.y, "leftDestinationY");
		InitializeDataMember(stream, m_leftDestination.z, "leftDestinationZ");
		InitializeDataMember(stream, m_rightDestination.x, "rightDestinationX");
		InitializeDataMember(stream, m_rightDestination.y, "rightDestinationY");
		InitializeDataMember(stream, m_rightDestination.z, "rightDestinationZ");
		m_State = g_AISYSTEM->serializeState(temp); 
	}

	void AI::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_State, "state");
		WriteDataMember(element, m_leftDestination.x, "leftDestinationX");
		WriteDataMember(element, m_leftDestination.y, "leftDestinationY");
		WriteDataMember(element, m_leftDestination.z, "leftDestinationZ");
		WriteDataMember(element, m_rightDestination.x, "rightDestinationX");
		WriteDataMember(element, m_rightDestination.y, "rightDestinationY");
		WriteDataMember(element, m_rightDestination.z, "rightDestinationZ");
	}

}