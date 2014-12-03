/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Action.h
Purpose: Header for Action.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/11/14
- End Header -----------------------------------------------------*/

#pragma once
#include "component.h"
#include <string>

namespace KrakEngine
{
	class Action :
		public Component
	{
	public:
		Action();
		~Action();
	
	virtual void SerializeViaXML(Serializer& stream);
	virtual void SerializeToXML(tinyxml2::XMLElement * element);
	
	void PerformAction1(GameObject * Go);
	void PerformAction2(GameObject * Go);

	void SetAction1(std::string newaction) {m_Action1 = newaction;}
	void SetAction2(std::string newaction) {m_Action2 = newaction;}

	private:
		std::string m_Action1;
		std::string m_Action2;
	};

};