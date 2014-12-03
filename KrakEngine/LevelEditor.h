/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: LevelEditor.h
Purpose: Header for LevelEditor.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: Go Ninja
Author: Jeff Kitts, j.kitts
Creation date: 3/1/2014
- End Header -----------------------------------------------------*/

#pragma once

#include "Precompiled.h"
//uses list

namespace KrakEngine{

	class GameObject;

	struct ObjectNode
	{
		GameObject * obj;
		ObjectNode * Next;
		ObjectNode * Prev;
	};

	class LevelEditor
	{
	public:
		LevelEditor(int level);
		~LevelEditor();

		void Update();
	
		void PlaceOnObjectTypeList(std::string name);

		void LoadObjectTypesScript();

		GameObject * GetCurrentObj() { return currentObj; }

	//	static int m_currentTexture;

	private:
		GameObject * currentObj;
		GameObject * Cursor;
		GameObject * Player;
		
		void UpdateCurrentObjectRequired();
		void UpdateNoCurrentObjectRequired();
		void UpdateNormal();

		void DeleteCurrentObject();
		void PlaceCurrentObject();

		void CreateObject();
		
		void TestLevel();

		void CycleObjects();
		void CycleObjectTypes();
	
		void Export();

		void SetSelectedTo(bool b);

		void FindAllVisibleObjects();

		void ResizeTrigger(XMFLOAT2 sizevec);

		void LoadTextures();
		void LoadModels();

		void ReadLevelFile(int level);

		XMFLOAT3 & VectorAddX(XMFLOAT3 & vector, float variable);
		XMFLOAT3 & VectorAddY(XMFLOAT3 & vector, float variable);
		XMFLOAT3 & VectorAddZ(XMFLOAT3 & vector, float variable);

		XMFLOAT2 & VectorAddX(XMFLOAT2 & vector, float variable);
		XMFLOAT2 & VectorAddY(XMFLOAT2 & vector, float variable);

		typedef std::vector<GameObject *> ObjectList;
		ObjectList AllGameObjectsInLevel;

		ObjectList AllObjectsVisible;

		typedef std::vector<std::string> ObjectTypeList;
		ObjectTypeList PossibleObjectTypes;
	
		std::string currentObjectType;

		int currentObjID;
		int objectTypeID;

		int timer1;
		int timer2;

		float viewsize;

		bool m_recentlymoved;

		int m_currentlevel;

		//AntTweakBar
		void BuildTweakBar();
		TwBar *myBar;
		
		void AddCurrentObjButtonsToTweakBar();
		void RemoveCurrentObjButtonsToTweakBar();

		int m_objectID;

	};
};