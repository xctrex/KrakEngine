//gameobject.h

//GameObject class holds all of a game object's components.

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------
#pragma once

#include <string>
#include <unordered_map>

#include "component.h"

namespace KrakEngine
{
	class GameObject
	{
	public:
		GameObject() : m_alive(true), m_type("") {};
		~GameObject();

		GameObject(const GameObject & go);
	
		void AddComponent(Component * comptoAdd);
		void ReplaceComponent(Component * comptoAdd);
	
		Component * GetComponent(ComponentType compname);
	
		std::string GetName() {return m_name;};
		void SetName(std::string Name) {m_name = Name;};

		int GetID() { return m_ObjectID; };
		void SetID(int id) { m_ObjectID = id; };

		void Kill() {m_alive = false;};
		bool GetAlive() {return m_alive;};
		
		void SetType(std::string type) {m_type = type;}
		std::string GetType() {return m_type;}

		void LoadType(Serializer & Seri);

		void WriteObjectToSerializer(Serializer & Seri);

		template<typename type>
		type* GetComponentType(ComponentType typeId)
		{
			return static_cast<type*>(GetComponent(typeId));
		}

		//map of components
		typedef std::unordered_map<ComponentType, Component *> ObjectComponentMap;
	private:
		
		//archetype of object
		std::string m_name;

		//type of object
		std::string m_type;

		//a unique identifier for the specifc instance
		int m_ObjectID;
		//a table/map/list of components "children"
		ObjectComponentMap ObjectComponents;

		//if the object is alive or needs to be deleted.
		bool m_alive;
	};


#define has(type) GetComponentType<type>( type##Comp );
	
};