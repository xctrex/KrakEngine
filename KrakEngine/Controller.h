/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Serializer.h
Header for Controller.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/11/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "component.h"
#include "ControllerSystem.h"

namespace KrakEngine
{
	class Controller :
		public Component
	{
	public:
		Controller() :  Component(ControllerComp), player(false), m_immobile(false) {
		
			g_CONTROLLERSYSTEM->ControllerList.emplace_back(this);
		};
		~Controller() {
		};
		
		bool IsCurrentController() const {return player;};
		void SetPlayerStatus(bool status) { player = status; };

		bool IsImmobile() const {return m_immobile; };

		void SerializeViaXML(Serializer & Seri);
		void SerializeToXML(tinyxml2::XMLElement * element);

	private:
		bool player;
		bool m_immobile;
	};

}