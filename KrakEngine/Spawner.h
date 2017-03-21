/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Spawner.h
Purpose:
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 3/7/14
- End Header -----------------------------------------------------*/


#pragma once
#include "component.h"
#include "LuaInterpreter.h"

namespace KrakEngine
{
	class Timer;

	class Spawner :
	public Component
	{
	public:
		Spawner();
		Spawner(int time, int mintime, int maxtime, std::string name);
		~Spawner();

		virtual void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);

		void Fire(float dt);

	private:
		bool m_RecentlyFired;
		std::string m_objScriptName;
		float m_time;
        float m_maxtime;
        float m_mintime;
	
		LuaInterpreter Li;

	
	};

}
