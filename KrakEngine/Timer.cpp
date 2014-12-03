#include "Timer.h"
#include "GamePlaySystem.h"
#include <iostream>

namespace KrakEngine
{

	Timer::Timer() : Component(TimerComp)
	{
		g_GAMEPLAYSYSTEM->TimerList.emplace_back(this);
	}

	Timer::Timer(int time) : m_time(time), Component(TimerComp)
	{
		g_GAMEPLAYSYSTEM->TimerList.emplace_back(this);
	}

	Timer::~Timer()
	{
	}

	void Timer::Tickdown()
	{
		m_time -= 1;
		if(m_time < 0 && m_time > -1000)
		{
	//		std::cout<<"Timer Expired."<<std::endl;
			Execute();
			m_time = -2000;
		}
	}

	void Timer::SerializeViaXML(Serializer & Seri)
	{

	}

};