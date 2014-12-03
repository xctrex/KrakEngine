/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Mechanics.cpp
Purpose: Used primarily to store game mechanics. These are mechanics more complicated than simply "create an object of type X at location X,Y,Z."
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/28/2014
- End Header -----------------------------------------------------*/

#include "Mechanics.h"
#include "Transform.h"
#include "RigidBody.h"
#include "Model.h"
#include "GameObject.h"
#include "Factory.h"
#include "ControllerSystem.h"
#include "Timer.h"
#include "GamePlaySystem.h"

namespace KrakEngine{

	Mechanics::Mechanics() : gobox(NULL)
	{
	}

	Mechanics::~Mechanics()
	{
	}

	void Mechanics::Update()
	{
		ids[0] = -1;
		ids[1] = -1;
		buttons = 0;
		
		if(m_TriggeredByButton)
		{
			Trigger * tc = (m_TriggeredByButton)->has(Trigger);
			tc->SetTriggeredOnButtonPress(1);
		}
	}

	void Mechanics::ButtonPressed(int id) {
		if(ids[0] == id || ids[1] == id)
			return;

		if(ids[0] == -1)
			ids[0] = id;
		else if(ids[1] == -1)
			ids[1] = id;
		else
			return;

		buttons++;

		Trigger * tc = (m_TriggeredByButton)->has(Trigger);
		tc->SetTriggeredOnButtonPress(buttons);

		Particle *pc = (m_TriggeredByButton)->has(Particle);
		
		if(buttons == 2)
			pc->SetEnabled(true);
		else
			pc->SetEnabled(false);
	}

	XMFLOAT3 Mechanics::AddPointToBox(float x, float y)
	{
		XMFLOAT3 position = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(XMFLOAT2(x, y));

		box.emplace_back(position);

		if(box.size() >= 4)
			CreateBox();

		return position;
	}

	void Mechanics::PlaceBox(float x, float y)
	{
		if(gobox)
		{
			gobox->Kill();
			gobox = NULL;
		}

		gobox = g_FACTORY->Create("mimebox");

		XMFLOAT3 position = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(XMFLOAT2(x, y));
		position.z = g_GAMEPLAYSYSTEM->GetCurrentTrack();
		
		Transform * tc = gobox->has(Transform);
	
	//	MimeBoxTimer * t = new MimeBoxTimer(1);
		
		//gobox->AddComponent(t);
		tc->SetPosition(position);
	}

	void Mechanics::CreateBox()
	{
		float minX, maxX, minY, maxY;
		float * curx, * cury;
		minX = minY = 100000.0f;
		maxX = maxY = -100000.0f;
		
		BoxList::iterator boxit = box.begin();
		for ( ; boxit != box.end(); ++boxit )
		{
			curx = &(*boxit).x;
			cury = &(*boxit).y;

			if(*curx < minX)
				minX = *curx;
			if(*cury < minY)
				minY = *cury;
			if(*curx > maxX)
				maxX = *curx;
			if(*cury > maxY)
				maxY = *cury;
		}

		float sizeX = maxX - minX;
		float sizeY = maxY - minY;

		GameObject * playa = g_CONTROLLERSYSTEM->FindController();
	
		GameObject * theBox = g_FACTORY->Create("object_misc");

		Transform * box_transform = theBox->has(Transform);
		
		Transform * player_transform = playa->has(Transform);

		XMFLOAT3 pos2 = player_transform->GetPosition();

		XMFLOAT3 pos(minX + sizeX/2, minY + sizeY/2, pos2.z);
		box_transform->SetPosition(pos);

		Model * mc = theBox->has(Model);
		mc->SetSize(sizeX, sizeY, 1);

		RigidBody *pc = theBox->has(RigidBody);
		pc->mass = 1.0f;

		pc->SetSize(sizeX, sizeY, 1);

		Timer * t = new DeathTimer(2000);

		theBox->AddComponent(t);

		box.clear();
	}

	void Mechanics::SetGoBox(GameObject * go)
	{
		gobox = go;
	}

};