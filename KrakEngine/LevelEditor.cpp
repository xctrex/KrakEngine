/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: LevelEditor.cpp
Purpose: In-engine level editor tool for building levels.
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: Go Ninja
Author: Jeff Kitts, j.kitts
Creation date: 3/1/2014
- End Header -----------------------------------------------------*/

#include "LevelEditor.h"

#include "InputSystem.h"
#include "Factory.h"
#include "GameObject.h"
#include "LuaInterpreter.h"

#include "AISystem.h"

#include "Transform.h"
#include "Model.h"
#include "RigidBody.h"
#include "Sprite.h"
#include "Trigger.h"
#include "Image.h"

#include "Core.h"

#include "Camera.h"

#include "Precompiled.h"

#include "GamePlaySystem.h"

#define DELTAMOVE 0.1f
#define MINSIZE 1.0f

#define VIEW_SIZE 20.0f

namespace KrakEngine{

	typedef std::vector<std::string> StringVector;
	StringVector Alltextures;
	StringVector Allmodels;
	int currentTexture = 0;
	int currentModel = 0;

	std::string objectName;

	GameObject * go;

	//TweakBar Callback functions
	void TW_CALL TextureNameCB(void *clientData)
	{ 
		Model * mc = go->has(Model);

		if(mc)
		{
			mc->SetTextureName(Alltextures[currentTexture]);
		}
	}

	void TW_CALL ModelNameCB(void *clientData)
	{ 
		Model * mc = go->has(Model);

		if(mc)
		{
			mc->SetModelName(Allmodels[currentModel]);
			mc->CreateBuffers(g_GRAPHICSSYSTEM->GetD3DDevice1());
		}
	}

	void TW_CALL ObjectNameCB(void *clientData)
	{
		go->SetName(objectName);
	}

	void TW_CALL CopyStdStringToClient(std::string& destinationClientString, const std::string& sourceLibraryString)
	{
	  destinationClientString = sourceLibraryString;
	}

	void TW_CALL SetMyStdStringCB(const void *value, void *clientData)
	{
	  // Set: copy the value of s3 from AntTweakBar
	  const std::string *srcPtr = static_cast<const std::string *>(value);
	  go->SetName(*srcPtr);
	}
	void TW_CALL GetMyStdStringCB(void *value, void * /*clientData*/)
	{
	  // Get: copy the value of s3 to AntTweakBar
	  std::string *destPtr = static_cast<std::string *>(value);
	  TwCopyStdStringToLibrary(*destPtr, go->GetName());
	}
	
	LevelEditor::LevelEditor(int level) : currentObj(NULL), currentObjID(-1), objectTypeID(0), Player(NULL), viewsize(VIEW_SIZE), m_currentlevel(level)
	{
		Cursor = g_FACTORY->GetObjList().FindByName("Cursor");

		ReadLevelFile(level);
		
		BuildTweakBar();

		g_WINDOWSYSTEM->ShowTheCursor(true);
	}

	LevelEditor::~LevelEditor()
	{
		TwTerminate();
		g_WINDOWSYSTEM->ShowTheCursor(false);
	}

	void LevelEditor::ReadLevelFile(int level)
	{
		std::string filepath = "Assets/Levels/";
		std::stringstream stream;
		
		stream<<filepath<<level;
		
		Serializer Seri;
		if(Seri.LoadFile(stream.str()))
		{
			Seri.LoadFirstItem();
			do
			{
				if(Seri.CheckAttribute("type", "gameobject"))
				{
					GameObject * go = g_FACTORY->Create(Seri, Seri.GetElementName());
					if(go->GetName() == "FPS" || go->GetName() == "Cursor")
						go->Kill();
					else if(go->GetName() == "player")
					{
						Player = go;
						AllGameObjectsInLevel.emplace_back(go);
					}
					else
						AllGameObjectsInLevel.emplace_back(go);
				}
			}
			while(Seri.NextNode());
		}

		if(!Player)
			Player = g_FACTORY->Create("player");
	}

	void LevelEditor::LoadTextures()
	{
		tinyxml2::XMLDocument txmlDoc;
        ThrowErrorIf(
            tinyxml2::XML_SUCCESS != txmlDoc.LoadFile("Assets\\Textures\\Textures.xml"), 
            "Failed to load Assets\\Textures\\Textures.xml"
            );
                
        // Loop through all the textures and load each one
        tinyxml2::XMLElement* textureElement = txmlDoc.FirstChildElement("Texture");
        while (textureElement != nullptr)
        {
			Alltextures.emplace_back(textureElement->Attribute("Name"));
            textureElement = textureElement->NextSiblingElement("Texture");
        }
	}
	
	void LevelEditor::LoadModels()
	{
		tinyxml2::XMLDocument txmlDoc;
        ThrowErrorIf(
            tinyxml2::XML_SUCCESS != txmlDoc.LoadFile("Assets\\Models\\Models.xml"), 
            "Failed to load Assets\\Models\\Models.xml"
            );
                
        // Loop through all the textures and load each one
        tinyxml2::XMLElement* modelElement = txmlDoc.FirstChildElement("Model");
        while (modelElement != nullptr)
        {
			Allmodels.emplace_back(modelElement->Attribute("Name"));
            modelElement = modelElement->NextSiblingElement("Model");
        }
	}

	void LevelEditor::LoadObjectTypesScript()
	{
		LuaInterpreter ScriptLoader;

		ThrowErrorIf(!ScriptLoader.LoadScript("leveleditor"), "script problems");	

		currentObjectType = PossibleObjectTypes.front();
	}

	//Add tweaks here to be shown when a currentobject is selected
	void LevelEditor::AddCurrentObjButtonsToTweakBar()
	{
		go = currentObj;

		m_objectID = go->GetID();
		objectName = go->GetName();
		
		TwAddButton(myBar, "Texture Name", TextureNameCB, NULL, " label='Texture' ");

		TwAddButton(myBar, "Model Name", ModelNameCB, NULL, " label='Model' ");
		TwAddVarRO(myBar, "Object ID", TW_TYPE_INT32, &m_objectID, " label='Object ID' ");

		TwAddSeparator(myBar, "object name", NULL);

		TwAddVarRW(myBar, "Name", TW_TYPE_STDSTRING, &objectName, NULL);
	}

	//Add removal functions for options only necessary when current object is selected
	void LevelEditor::RemoveCurrentObjButtonsToTweakBar()
	{
		TwRemoveVar(myBar, "Texture Name");
		TwRemoveVar(myBar, "Object ID");
		TwRemoveVar(myBar, "Object Name");
		TwRemoveVar(myBar, "Name");
		TwRemoveVar(myBar, "Model Name");
	}

	void LevelEditor::Update()
	{
		if(timer1 > 0)
			timer1--;
		if(timer2 > 0)
			timer2--;


		//Keypresses that require an object selected
		if(currentObj)
		{
			AddCurrentObjButtonsToTweakBar();
		
			UpdateCurrentObjectRequired();
		}
		//keypresses that require no object selected
		else
		{
			RemoveCurrentObjButtonsToTweakBar();
		
			UpdateNoCurrentObjectRequired();
		}
		//keypresses that do not care if an object is selected or not

		UpdateNormal();
			
	}

	void LevelEditor::UpdateNormal()
	{
		if(g_INPUTSYSTEM->IsKeyDown(DIK_ESCAPE))
		{
			CORE->SetNextState(GameStates::Quit);
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_N))
		{
			Export();
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_M))
		{
			TestLevel();
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_UP))
		{
			Transform * tc = Cursor->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.y += DELTAMOVE;
			tc->SetPosition(posvec);

			m_recentlymoved = true;
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_DOWN))
		{
			Transform * tc = Cursor->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.y -= DELTAMOVE;
			tc->SetPosition(posvec);

			m_recentlymoved = true;
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_LEFT))
		{
			Transform * tc = Cursor->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.x -= DELTAMOVE;
			tc->SetPosition(posvec);
			
			m_recentlymoved = true;
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_RIGHT))
		{
			Transform * tc = Cursor->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.x += DELTAMOVE;
			tc->SetPosition(posvec);

			m_recentlymoved = true;
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_EQUALS))
		{
			Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

			c->SetDepth(c->GetDepth() + DELTAMOVE);

			viewsize -= DELTAMOVE;

			m_recentlymoved = true;
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_MINUS))
		{
			Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

			c->SetDepth(c->GetDepth() - DELTAMOVE);

			viewsize += DELTAMOVE;

			m_recentlymoved = true;
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_1))
		{
			CycleObjects();
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_2))
		{
			CycleObjectTypes();
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_3))
		{
			currentObj = g_FACTORY->GetObjList().FindByName("train");
		}
		
		if(g_INPUTSYSTEM->IsMouseButtonDown(Left))
		{				
			POINT p;
			GetCursorPos(&p);
		
			ScreenToClient( g_GRAPHICSSYSTEM->GetHWND(), &p);
			
			XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();

	
			XMFLOAT2 point;

			point.x = (float)p.x;
			point.y = (float)p.y;

			point.x /= (float)screensize.x;
			point.y /= (float)screensize.y;

			XMFLOAT3 position = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(point);
			
			std::cout<<position.x<<" "<<position.y<<" "<<std::endl;
			

			ObjectList::iterator it = AllGameObjectsInLevel.begin();
			for ( ; it != AllGameObjectsInLevel.end(); ++it)
			{
				Transform *tc = (*it)->has(Transform);
				if(!tc) continue;

				XMFLOAT3 pos = tc->GetPosition();

				Model *mc = (*it)->has(Model);
				RigidBody *pc = (*it)->has(RigidBody);
				Sprite *sc = (*it)->has(Sprite);

				XMFLOAT3 size;

				if(mc)
				{
					size = mc->GetSize();
				}
				if(pc)
				{
					size = pc->GetSize();
				}
				if(sc)
				{
					XMFLOAT2 spritesize = sc->GetSize();
					size = XMFLOAT3(spritesize.x, spritesize.y, 0.0f);
				}

				float minX = pos.x - size.x/2.0f;
				float maxX = pos.x + size.x/2.0f;

				float minY = pos.y - size.y/2.0f;
				float maxY = pos.y + size.y/2.0f;

				if( minX < position.x &&
					maxX > position.x &&
					minY < position.y &&
					maxY > position.y )
				{
					if(currentObj == *it)
						continue;

					if(currentObj)
						SetSelectedTo(false);
					
					currentObj = *it;
					SetSelectedTo(true);
					break;
				}
			}
		}
	}

	void LevelEditor::UpdateCurrentObjectRequired()
	{
		if(g_INPUTSYSTEM->IsKeyDown(DIK_W))
		{
			Transform *tc = currentObj->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.y += DELTAMOVE;
			tc->SetPosition(posvec);

			Particle *pc = currentObj->has(Particle);
			if(pc)
			{
				posvec = pc->getCenter();
				posvec.y += DELTAMOVE;
				pc->setCenter(posvec);
			}

			Sprite *sc = currentObj->has(Sprite);
			if(sc)
			{
				ResizeTrigger(sc->GetSize());
			}

		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_A))
		{
			Transform *tc = currentObj->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.x -= DELTAMOVE;
			tc->SetPosition(posvec);

			Particle *pc = currentObj->has(Particle);
			if(pc)
			{
				posvec = pc->getCenter();
				posvec.x -= DELTAMOVE;
				pc->setCenter(posvec);
			}

			
			Sprite *sc = currentObj->has(Sprite);
			if(sc)
			{
				ResizeTrigger(sc->GetSize());
			}
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_S))
		{
			Transform *tc = currentObj->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.y -= DELTAMOVE;
			tc->SetPosition(posvec);
			
			Particle *pc = currentObj->has(Particle);
			if(pc)
			{
				posvec = pc->getCenter();
				posvec.y -= DELTAMOVE;
				pc->setCenter(posvec);
			}

			
			Sprite *sc = currentObj->has(Sprite);
			if(sc)
			{
				ResizeTrigger(sc->GetSize());
			}
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_D))
		{
			Transform *tc = currentObj->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.x += DELTAMOVE;
			tc->SetPosition(posvec);

			Particle *pc = currentObj->has(Particle);
			if(pc)
			{
				posvec = pc->getCenter();
				posvec.x += DELTAMOVE;
				pc->setCenter(posvec);
			}

			Sprite *sc = currentObj->has(Sprite);
			if(sc)
			{
				ResizeTrigger(sc->GetSize());
			}
			

		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_Q))
		{
			Transform *tc = currentObj->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.z -= DELTAMOVE;
			tc->SetPosition(posvec);
		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_E))
		{
			Transform *tc = currentObj->has(Transform);

			XMFLOAT3 posvec = tc->GetPosition();
			posvec.z += DELTAMOVE;
			tc->SetPosition(posvec);
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_J))
		{
			Model *mc = currentObj->has(Model);
			RigidBody *pc = currentObj->has(RigidBody);
			Sprite *sc = currentObj->has(Sprite);
	

			if(mc)
			{
				XMFLOAT3 sizevec = mc->GetSize();
				mc->SetSize(VectorAddX(sizevec, -DELTAMOVE));
			}
			if(pc)
			{
				XMFLOAT3 sizevec = pc->GetSize();
				pc->SetSize(VectorAddX(sizevec, -DELTAMOVE));
			}
			if(sc)
			{
				XMFLOAT2 sizevec = sc->GetSize();
				sc->SetSize(VectorAddX(sizevec, -DELTAMOVE));

				ResizeTrigger(sizevec);
			}

		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_L))
		{
			Model *mc = currentObj->has(Model);
			RigidBody *pc = currentObj->has(RigidBody);
			Sprite *sc = currentObj->has(Sprite);

			if(mc)
			{
				XMFLOAT3 sizevec = mc->GetSize();
				mc->SetSize(VectorAddX(sizevec, DELTAMOVE));
			}
			if(pc)
			{
				XMFLOAT3 sizevec = pc->GetSize();
				pc->SetSize(VectorAddX(sizevec, DELTAMOVE));
			}
			if(sc)
			{
				XMFLOAT2 sizevec = sc->GetSize();
				sc->SetSize(VectorAddX(sizevec, DELTAMOVE));

				ResizeTrigger(sizevec);
			}
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_I))
		{
			Model *mc = currentObj->has(Model);
			RigidBody *pc = currentObj->has(RigidBody);
			Sprite *sc = currentObj->has(Sprite);

			if(mc)
			{
				XMFLOAT3 sizevec = mc->GetSize();
				mc->SetSize(VectorAddY(sizevec, DELTAMOVE));
			}
			if(pc)
			{
				XMFLOAT3 sizevec = pc->GetSize();
				pc->SetSize(VectorAddY(sizevec, DELTAMOVE));
			}
			if(sc)
			{
				XMFLOAT2 sizevec = sc->GetSize();
				sc->SetSize(VectorAddY(sizevec, DELTAMOVE));

				ResizeTrigger(sizevec);
			}
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_K))
		{
			Model *mc = currentObj->has(Model);
			RigidBody *pc = currentObj->has(RigidBody);
			Sprite *sc = currentObj->has(Sprite);

			if(mc)
			{
				XMFLOAT3 sizevec = mc->GetSize();
				mc->SetSize(VectorAddY(sizevec, -DELTAMOVE));
			}
			if(pc)
			{
				XMFLOAT3 sizevec = pc->GetSize();
				pc->SetSize(VectorAddY(sizevec, -DELTAMOVE));
			}
			if(sc)
			{
				XMFLOAT2 sizevec = sc->GetSize();
				sc->SetSize(VectorAddY(sizevec, -DELTAMOVE));

				ResizeTrigger(sizevec);
			}
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_O))
		{
			Model *mc = currentObj->has(Model);
			RigidBody *pc = currentObj->has(RigidBody);

			if(mc)
			{
				XMFLOAT3 sizevec = mc->GetSize();
				mc->SetSize(VectorAddZ(sizevec, DELTAMOVE));
			}
			if(pc)
			{
				XMFLOAT3 sizevec = pc->GetSize();
				pc->SetSize(VectorAddZ(sizevec, DELTAMOVE));
			}
		}

		if(g_INPUTSYSTEM->IsKeyDown(DIK_U))
		{
			Model *mc = currentObj->has(Model);
			RigidBody *pc = currentObj->has(RigidBody);

			if(mc)
			{
				XMFLOAT3 sizevec = mc->GetSize();
				mc->SetSize(VectorAddZ(sizevec, -DELTAMOVE));
			}
			if(pc)
			{
				XMFLOAT3 sizevec = pc->GetSize();
				pc->SetSize(VectorAddZ(sizevec, -DELTAMOVE));
			}

		}
		if(g_INPUTSYSTEM->IsKeyDown(DIK_V))
		{
			PlaceCurrentObject();
		}


		if(g_INPUTSYSTEM->IsKeyDown(DIK_X))
		{
			DeleteCurrentObject();
		}
	}

	void LevelEditor::UpdateNoCurrentObjectRequired()
	{
		if(g_INPUTSYSTEM->IsKeyDown(DIK_C))
		{
			currentObj = g_FACTORY->Create(currentObjectType);
			SetSelectedTo(true);

			Transform *tc = currentObj->has(Transform);

			Transform *cursor_tran = Cursor->has(Transform);
			tc->SetPosition(cursor_tran->GetPosition());		
		}	
	}


	void LevelEditor::DeleteCurrentObject()
	{
		if(currentObj == Player)
			return;

		ObjectList::iterator it = AllGameObjectsInLevel.begin();
		for( ; it != AllGameObjectsInLevel.end(); ++it)
		{
			if(*it == currentObj)
			{
				AllGameObjectsInLevel.erase(it);
				return;
			}
		}

		currentObj->Kill();
		currentObj = NULL;		
	}

	void LevelEditor::PlaceCurrentObject()
	{
		SetSelectedTo(false);

		ObjectList::iterator it = AllGameObjectsInLevel.begin();
		for( ; it != AllGameObjectsInLevel.end(); ++it)
		{
			if(*it == currentObj)
			{
				currentObj = NULL;
				return;
			}
		}

		AllGameObjectsInLevel.emplace_back(currentObj);
		currentObjID++;
		currentObj = NULL;
	}

	void LevelEditor::TestLevel()
	{
		g_PHYSICSSYSTEM->UnpausePhysics();
		g_AISYSTEM->UnpauseAI();

		CORE->SetNextLevel(m_currentlevel);

		g_GRAPHICSSYSTEM->HideTriggers();
		CORE->SetNextState(GameStates::InGame);
		delete this;
	}

	void LevelEditor::Export()
	{
		Serializer Seri;
		
		std::string filepath = "Assets/Levels/";
		std::stringstream stream;
		
		stream<<filepath<<m_currentlevel;
		

		if(Seri.LoadFile(stream.str()))
			Seri.ClearFile();

		Seri.PublishLevel();

		ObjectList::iterator it = AllGameObjectsInLevel.begin();
		for( ; it != AllGameObjectsInLevel.end(); ++it )
		{
			Seri.WriteGameObjectToFile(*it);
		}

		Seri.SaveFile();
	}

	void LevelEditor::CycleObjects()
	{
		if(AllGameObjectsInLevel.empty()) return;
		if(timer1 > 0) return;

		if(m_recentlymoved)
		{
			FindAllVisibleObjects();
		}

		m_recentlymoved = false;

		if(AllObjectsVisible.empty()) return;

		if(currentObj)
		{
			SetSelectedTo(false);
		}

		int i = currentObjID + 1;

		while(i != currentObjID)
		{
			i %= AllObjectsVisible.size();

			if(AllObjectsVisible[i])
			{
				currentObj = AllObjectsVisible[i];
				currentObjID = i;

				SetSelectedTo(true);
				break;
			}

			++i;
		}

		timer1 = 100;
	}

	void LevelEditor::CycleObjectTypes()
	{
		if(PossibleObjectTypes.empty()) return;

		if(timer2 <= 0)
		{
			int i = objectTypeID + 1;

			while(i != objectTypeID)
			{
				i %= PossibleObjectTypes.size();

				currentObjectType = PossibleObjectTypes[i];
				objectTypeID = i;
				break;

				//++i;	//Cristina 20140324 Unreachable code. What are we trying to do here?
			}

			timer2 = 100;
		}
	}

	void LevelEditor::SetSelectedTo(bool b)
	{
		Model * mc = currentObj->has(Model)
			if(mc)
				mc->SetSelected(b);
		Sprite * sc = currentObj->has(Sprite)
			if(sc)
				sc->SetSelected(b);
	}
	
	void LevelEditor::PlaceOnObjectTypeList(std::string name)
	{
		PossibleObjectTypes.emplace_back(name);
	}

	XMFLOAT3 & LevelEditor::VectorAddX(XMFLOAT3 & vector, float toAdd)
	{
		vector.x += toAdd;
		if(vector.x < MINSIZE)
			vector.x = MINSIZE;

		return vector;
	}

	XMFLOAT2 & LevelEditor::VectorAddX(XMFLOAT2 & vector, float toAdd)
	{
		vector.x += toAdd;
		if(vector.x < MINSIZE)
			vector.x = MINSIZE;

		return vector;
	}

	XMFLOAT3 & LevelEditor::VectorAddY(XMFLOAT3 & vector, float toAdd)
	{
		vector.y += toAdd;
		if(vector.y < MINSIZE)
			vector.y = MINSIZE;

		return vector;
	}

	XMFLOAT2 & LevelEditor::VectorAddY(XMFLOAT2 & vector, float toAdd)
	{
		vector.y += toAdd;
		if(vector.y < MINSIZE)
			vector.y = MINSIZE;

		return vector;
	}

	XMFLOAT3 & LevelEditor::VectorAddZ(XMFLOAT3 & vector, float toAdd)
	{
		vector.z += toAdd;
		if(vector.z < MINSIZE)
			vector.z = MINSIZE;

		return vector;
	}

	void LevelEditor::ResizeTrigger(XMFLOAT2 sizevec)
	{
		Trigger *tc = currentObj->has(Trigger);
		if(!tc) return;

		Transform * tran = currentObj->has(Transform);
				
		XMFLOAT3 position = tran->GetPosition();

		XMFLOAT3 start = position;
		XMFLOAT3 end = position;

		start.x -= sizevec.x / 2.0f;
		start.y -= sizevec.y / 2.0f;

		end.x += sizevec.x / 2.0f;
		end.y += sizevec.y / 2.0f;

		tc->SetStartPosition(start);
		tc->SetEndPosition(end);
			
	}

	void LevelEditor::FindAllVisibleObjects()
	{
		AllObjectsVisible.clear();
		Transform * cursortc = Cursor->has(Transform);

		XMFLOAT3 cursorpos = cursortc->GetPosition();
		
		ObjectList::iterator it = AllGameObjectsInLevel.begin();
		for( ; it != AllGameObjectsInLevel.end(); ++it)
		{
			Transform * tc = (*it)->has(Transform);
			if (!tc) continue;

			XMFLOAT3 objvec = tc->GetPosition();
			
			if( objvec.x < cursorpos.x + viewsize
				&& objvec.x > cursorpos.x - viewsize 
				&& objvec.y < cursorpos.y + viewsize
				&& objvec.y > cursorpos.y - viewsize
				)
				AllObjectsVisible.emplace_back(*it);
		}
	}
	
	void LevelEditor::BuildTweakBar()
	{
		TwInit(TW_DIRECT3D11, g_GRAPHICSSYSTEM->GetD3DDevice1().Get());
		TwCopyStdStringToClientFunc(CopyStdStringToClient);

		XMUINT2 windowsize = g_GRAPHICSSYSTEM->GetWindowSize();
		TwWindowSize(windowsize.x, windowsize.y);
	
		myBar = TwNewBar("TextureBar");

		TwType TextureList;
		TwType ModelList;
		
		const char * listoftextures;
		const char * listofmodels;

		if(Alltextures.empty())
			LoadTextures();
	
		if(Allmodels.empty())
			LoadModels();

		StringVector::iterator ittex = Alltextures.begin();
		std::string textures;

		for ( ; ittex != Alltextures.end(); ++ittex)
		{
			textures += *ittex;
			textures += ",";
		}
		
		StringVector::iterator itmod = Allmodels.begin();
		std::string models;

		for ( ; itmod != Allmodels.end(); ++itmod)
		{
			models += *itmod;
			models += ",";
		}
	
		listoftextures = textures.c_str();
		listofmodels = models.c_str();

		TextureList = TwDefineEnumFromString("Textures", listoftextures);
		TwAddVarRW(myBar, "Textures", TextureList, &currentTexture, NULL);

		ModelList = TwDefineEnumFromString("Models", listofmodels);
		TwAddVarRW(myBar, "Models", ModelList, &currentModel, NULL);
	};
	
};