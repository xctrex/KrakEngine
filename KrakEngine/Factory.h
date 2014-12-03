/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Factory.h
Purpose: Header for Factory.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 1/12/14
- End Header -----------------------------------------------------*/

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "ISystem.h"

#include "serializer.h"
#include "component.h"
#include "componentfactory.h"

namespace KrakEngine
{
	class InstanceVector
	{
	public:
		InstanceVector() {VectorOfGameObjects.resize(100);};
		~InstanceVector() {};

		int PushBack(GameObject * Go);
		GameObject * FindByID(int id);
		GameObject * FindByName(std::string name);
		GameObject * FindByType(std::string type);
		std::list<GameObject *> & FindByTypeList(std::string type);

		typedef std::vector<GameObject *> GameObjectVector;
		typedef GameObjectVector::iterator InstanceVectorIterator;

		InstanceVectorIterator GetBeginOfInstanceVector() {return VectorOfGameObjects.begin(); };
		InstanceVectorIterator GetEndOfInstanceVector() {return VectorOfGameObjects.end(); };

	private:
		//this is used to hold all instances.
		GameObjectVector VectorOfGameObjects;
		std::list<GameObject *> ListOfCurrentObj;
	};

	class Factory: public ISystem
	{
	public:
		Factory();
		~Factory() {};

		//create an object
		GameObject * Create(std::string name);
		GameObject * Create(Serializer & Seri, std::string name);
		GameObject * Create(GameObject *go);

		//it deletes all objects that are declared "dead".
		virtual void Update(float dt);
		
		//no messages to handle
		virtual void HandleMessages(Message* message) {};
		
		//RAII
		virtual bool Initialize() {return true;};
		virtual bool Shutdown() {return true;};

		void DeleteAllObjects();

		std::string GetNameFromComponentType(ComponentType ct); 

		GameObject * FindByID(int id) {return VectorOfInstances.FindByID(id); };	
		GameObject * FindByType(std::string type) {return VectorOfInstances.FindByType(type); };
		std::list<GameObject *> & FindByTypeList(std::string type) {return VectorOfInstances.FindByTypeList(type); };


		InstanceVector & GetObjList() {return VectorOfInstances;};

		//may be used to preload objects.
		typedef std::unordered_map<std::string, GameObject *> ArchetypeMap;

		

	private:
		//replaces a component if one is listed in the level file.
		void Factory::AddComponents(Serializer & Seri, GameObject * GO);

		//Creates components
		ComponentFactory CompFactory;
		Component * CreateComponent(ComponentType compname);
		Component * CreateComponent(std::string compname);

		GameObject * CreateNewObject(Serializer & Seri, std::string objName);

		void Dispatch(GameObject * Go);

		//copying pre-existing objects.
		ArchetypeMap MapOfArchetypes;

		InstanceVector VectorOfInstances;		
	};
	extern Factory* g_FACTORY;
};