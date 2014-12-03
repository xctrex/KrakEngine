//gameobject.cpp

//GameObject class holds all of a game object's components.

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------
#include "Precompiled.h"
#include "gameobject.h"

#include "Factory.h"

namespace KrakEngine
{

	GameObject::~GameObject()
	{
		ObjectComponentMap::iterator it = ObjectComponents.begin();
		for ( ; it != ObjectComponents.end(); ++it )
		{
			delete it->second;
		}
	}

	//GameObject::GameObject(const GameObject & go)
	//{
	//	ObjectComponentMap::const_iterator it = go.ObjectComponents.begin();
	//	for ( ; it != go.ObjectComponents.end(); ++it )
	//	{
	//		this->AddComponent(it->second->Clone());
	//	}
	//}

	//GetComponent
	//Finds a Component of the given type. If none is found, returns NULL.
	//--------------------------------------------------------------------------------
	Component * GameObject::GetComponent(ComponentType name)
	{
		ObjectComponentMap::iterator it = ObjectComponents.find(name);
		if (it == ObjectComponents.end()) return NULL;
		
		return it->second;
	}
	
	//AddComponent
	//Adds a component to the object's component map.
	//--------------------------------------------------------------------------------
	void GameObject::AddComponent(Component * comptoAdd)
	{
		//already has a component of this type.
		//if (GetComponent(comptoAdd->GetType())) 
		//	return;

		ObjectComponents.emplace(comptoAdd->GetType(), comptoAdd);

		comptoAdd->SetParent(this);
	}

	//ReplaceComponent
	//Finds the existing component in the map, removes it, and replaces it with a
	//new component.
	//--------------------------------------------------------------------------------
	void GameObject::ReplaceComponent(Component * comptoAdd)
	{
		ObjectComponents.emplace(comptoAdd->GetType(), comptoAdd);

		comptoAdd->SetParent(this);
	}

	//LoadType
	//Sets the specific type of object (I am a Fireman, or a Hat, etc).
	//--------------------------------------------------------------------------------
	void GameObject::LoadType(Serializer & Seri)
	{
		if (Seri.CheckAttribute("objtype"))
			m_type = Seri.GetAttributeString("objtype");
	}

	//--------------------------------------------------------------------------------
	void GameObject::WriteObjectToSerializer(Serializer & stream)
	{
		ObjectComponentMap::iterator it = ObjectComponents.begin();
		for ( ; it != ObjectComponents.end(); ++it)
		{
			Component * c = it->second;

			std::string ComponentType = g_FACTORY->GetNameFromComponentType(c->GetType());
			tinyxml2::XMLElement * component = stream.GetFile()->NewElement(ComponentType.c_str());
			component->SetAttribute("type", "component");
			c->SerializeToXML(component);

			stream.GetElement()->InsertEndChild(component);
		}
	}

};