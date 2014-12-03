/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Mechanics.h
Purpose: Header for Mechanics.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/28/2014
- End Header -----------------------------------------------------*/

#pragma once

#include <list>
#include "Precompiled.h"
#include "GameObject.h"

#include <iostream>

namespace KrakEngine{
	
	class Mechanics
	{
	public:
		Mechanics();
		~Mechanics();

		void Update();

		//Mime
		XMFLOAT3 AddPointToBox(float x, float y);
		void CreateBox();

		void PlaceBox(float x, float y);

		void SetGoBox(GameObject * go);

		void ButtonPressed(int id);

		void SetTriggeredByButton(GameObject * go) { m_TriggeredByButton = go; };
	private:

		//for mime's box
		typedef std::list<XMFLOAT3> BoxList;
		BoxList box;

		GameObject * gobox;

		//for buttons
		int buttons;
		int ids[2];

		GameObject * m_TriggeredByButton;
	};

}