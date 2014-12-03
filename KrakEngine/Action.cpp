/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Action.cpp
Purpose: An Action component that tells what the object is and how it should act if the player uses it.
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/11/14
- End Header -----------------------------------------------------*/

#include "Action.h"
#include "LuaInterpreter.h"

#include "GameObject.h"

namespace KrakEngine
{
	Action::Action() : Component(ActionComp)
	{
	}

	Action::~Action()
	{
	}

	void Action::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, m_Action1, "Action1");
		InitializeDataMember(stream, m_Action2, "Action2");
	}

	void Action::SerializeToXML(tinyxml2::XMLElement * element){
		WriteDataMember(element, m_Action1, "Action1");
		WriteDataMember(element, m_Action2, "Action2");
	}

	void Action::PerformAction1(GameObject * Go)
	{
		LuaInterpreter Li;

		Li.SetCurrentObj(Go);

		Li.LoadScript(m_Action1);
	}

	void Action::PerformAction2(GameObject * Go)
	{
		LuaInterpreter Li;

		Li.SetCurrentObj(Go);

		Li.LoadScript(m_Action2);
	}

};