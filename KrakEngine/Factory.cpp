/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Factory.cpp
Purpose: A factory class that creates all the game objects used by the game.
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 1/12/14
- End Header -----------------------------------------------------*/
#include "Precompiled.h"
#include "factory.h"
#include <iostream>
#include "GameObject.h"

#include "Core.h"

namespace KrakEngine
{
	Factory* g_FACTORY = NULL;

	Factory::Factory()
	{
		g_FACTORY = this;
	}

	//Create
	//Creates an object from an object file
	//--------------------------------------------------------------------------------
	GameObject * Factory::Create(std::string objName)
	{
		Serializer Seri;
		std::string filename = "Assets/Objects/" + objName;

		if(!Seri.LoadFile(filename)) 
			return NULL;

		Seri.LoadFirstItem();
	
		return CreateNewObject(Seri, objName);
	}

	//Create
	//Creates an object from an already opened file (like a level file)
	//--------------------------------------------------------------------------------
	GameObject * Factory::Create(Serializer & Seri, std::string objName)
	{
		GameObject * GO = Create(objName);
		if(GO)
		{
			//check for components to replace
			if (!Seri.HasNoChildren())
			{
				Seri.PutFirstChildOnElement();
				AddComponents(Seri, GO);
			}

			GO->LoadType(Seri);
			return GO;
		}

		Seri.PutFirstChildOnElement();

		return CreateNewObject(Seri, objName);
	}

	//CreateNewObject
	//--------------------------------------------------------------------------------
	GameObject * Factory::CreateNewObject(Serializer & Seri, std::string objName)
	{
		GameObject * Go = new GameObject();
		//puts the object on the instance vector and gives it its unique id.
		Go->SetID(VectorOfInstances.PushBack(Go));

		AddComponents(Seri, Go);
		Go->SetName(objName);
		Go->LoadType(Seri);



		return Go;
	}


	//AddComponents
	//fills an object with components
	//--------------------------------------------------------------------------------
	void Factory::AddComponents(Serializer & Seri, GameObject * GO)
	{
		do
		if (Seri.CheckAttribute("type", "component"))
		{
			//std::cout<<Seri.GetElementName()<<std::endl;

			ComponentType comptype = CompFactory.GetComponentTypeFromString(Seri.GetElementName());
			Component * c = GO->GetComponent(comptype);

			if(c)
			{
				c->SerializeViaXML(Seri);
				GO->ReplaceComponent(c);
			}
			else
			{
				c = CreateComponent(Seri.GetElementName());
				//std::cout<<Seri.GetElementName()<<std::endl;
				if(c)
				{
					c->SerializeViaXML(Seri);
					GO->AddComponent(c);
				}
			}
		}
		while (Seri.NextNode());
	}

	//CreateComponent
	//Creates a component from a typeID
	//--------------------------------------------------------------------------------
	Component * Factory::CreateComponent(ComponentType compname)
	{
		return CompFactory.CreateComponent(compname);		
	}

	//CreateComponent
	//Creates a component from a string name
	//--------------------------------------------------------------------------------
	Component * Factory::CreateComponent(std::string compname)
	{
		return CompFactory.CreateComponent(compname);		
	}

	std::string Factory::GetNameFromComponentType(ComponentType ct)
	{
		return CompFactory.GetNameFromComponentType(ct);
	}

	//Update
	//Each frame it deletes all objects that are considered "dead".
	//--------------------------------------------------------------------------------
	void Factory::Update(float dt)
	{
		InstanceVector::InstanceVectorIterator it = VectorOfInstances.GetBeginOfInstanceVector();
		InstanceVector::InstanceVectorIterator endIT = VectorOfInstances.GetEndOfInstanceVector();

		for( ; it != endIT; ++it)
		{
			if(*it == NULL) continue;
			if(!(*it)->GetAlive())
			{
				ObjectDestroyedMessage ODM((*it)->GetID());
				CORE->Broadcast(&ODM);

				delete *it;
				*it = NULL;
			}
		}
	}

	//DeleteAllObjects
	//deletes all objects
	//--------------------------------------------------------------------------------
	void Factory::DeleteAllObjects()
	{
		InstanceVector::InstanceVectorIterator beginIT = VectorOfInstances.GetBeginOfInstanceVector();
		InstanceVector::InstanceVectorIterator endIT = VectorOfInstances.GetEndOfInstanceVector();

		ClearComponentListsMessage CCLM;

		CORE->Broadcast(&CCLM);

		for( ; beginIT != endIT; ++beginIT)
		{
				delete *beginIT;
				*beginIT = NULL;
		}
	}


	//PushBack
	//Puts a GameObject onto an instance list and returns the ID to the object.
	//--------------------------------------------------------------------------------
	int InstanceVector::PushBack(GameObject * Go)
	{
		int id = 0;

		//search for the first availible slot
		for (; id < (int)VectorOfGameObjects.size(); ++id)
		{
			if(VectorOfGameObjects[id] == NULL)
			{
				VectorOfGameObjects[id] = Go;
				return id;
			}
		}

		//if we are here, there are no availible slots. Increase the size of the vector and add 
		//to the end.
		id = VectorOfGameObjects.size();
		
		VectorOfGameObjects.resize(VectorOfGameObjects.size() + 100);
		VectorOfGameObjects[id] = Go;

		return id;
	}

	//RetrieveByID
	//Returns a pointer to the object from its ID number, or NULL if id is invalid.
	//--------------------------------------------------------------------------------
	GameObject * InstanceVector::FindByID(int id)
	{
		return VectorOfGameObjects[id];
	}

	//RetireveByName
	//Returns a pointer to the FIRST object of a given name, or null if there are none.
	//Useful for finding unique objects (like the player)
	//--------------------------------------------------------------------------------
	GameObject * InstanceVector::FindByName(std::string name)
	{
		for(InstanceVectorIterator it = GetBeginOfInstanceVector();
			it != GetEndOfInstanceVector(); 
			++it)
		{
			if(*it == NULL) continue;
			if((*it)->GetName() == name)
				return *it;
		}

        return NULL;
	}

	//Find by ObjectType
	GameObject * InstanceVector::FindByType(std::string name)
	{
		for(InstanceVectorIterator it = GetBeginOfInstanceVector();
			it != GetEndOfInstanceVector(); 
			++it)
		{
			if(*it == NULL) continue;
			if((*it)->GetType() == name)
				return *it;
		}

		return NULL;
	}

	std::list<GameObject *> & InstanceVector::FindByTypeList(std::string name)
	{
		ListOfCurrentObj.clear();

		for(InstanceVectorIterator it = GetBeginOfInstanceVector();
			it != GetEndOfInstanceVector(); 
			++it)
		{
			if(*it == NULL) continue;
			if((*it)->GetType() == name)
				ListOfCurrentObj.emplace_back(*it);
		}

		return ListOfCurrentObj;
	}


};