/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Spawner.cpp
Purpose:
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 3/7/14
- End Header -----------------------------------------------------*/

#include "Spawner.h"
#include "GameObject.h"
#include "Timer.h"
#include "GamePlaySystem.h"
#include "Factory.h"

namespace KrakEngine
{
	Spawner::Spawner(): Component(SpawnerComp), 
		m_time(100),
		m_mintime(0),
		m_maxtime(1),
		m_RecentlyFired(false)
	{
		g_GAMEPLAYSYSTEM->SpawnerList.emplace_back(this);
	}

	Spawner::Spawner(int time, int mintime, int maxtime, std::string name): Component(SpawnerComp), 
		m_objScriptName(name), 
		m_time(time), 
		m_mintime(mintime),
		m_maxtime(maxtime),
		m_RecentlyFired(false)
	{
		g_GAMEPLAYSYSTEM->SpawnerList.emplace_back(this);
	}

	Spawner::~Spawner()
	{
	}

	void Spawner::SerializeViaXML(Serializer& stream)
	 {
		InitializeDataMember(stream, m_objScriptName, "Script");
		InitializeDataMember(stream, m_mintime, "mintime");
		InitializeDataMember(stream, m_maxtime, "maxtime");
	};

	 void Spawner::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_objScriptName, "Script");
		WriteDataMember(element, m_mintime, "mintime");
		WriteDataMember(element, m_maxtime, "maxtime");
	};

	void Spawner::Fire(float dt)
	{
		m_time -= dt;

		if(m_time <= 0)
			m_RecentlyFired = false;

		if(m_RecentlyFired) return;
		
		Li.SetCurrentObj(GetOwner());

		Li.LoadScript(m_objScriptName);

		m_RecentlyFired = true;

		if( m_maxtime == m_mintime )
		{
			m_time = m_maxtime;
		}
		else
			m_time = (float)( rand() % (int)m_maxtime + (int)m_mintime);

	}

	
}