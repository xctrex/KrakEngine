//component.h

//the component base class that all components inherit, has functionality
//for all components.

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------
#pragma once

#include "serializer.h"

#include <unordered_map>
#include <string>

namespace KrakEngine
{
	enum ComponentType
{
	InvalidComp = 0,
	TransformComp,
	SpriteComp,
	LifeComp,
	AnimationComp,
	RigidBodyComp,
    ModelComp,
    CameraComp,
    TextComp,
    ImageComp,
	ControllerComp,
	BehaviorComp,
	ActionComp,
	TimerComp,
	AIComp,
	AudioComp,
	SpawnerComp,
	TriggerComp,
	ParticleComp
	};


	class GameObject;
	class Component
	{
	public:
		Component() {};
		Component(ComponentType type) : m_type(type) {};
		~Component() {};

		///Each component knows how to serialize itself.
		virtual void SerializeViaXML(Serializer & Seri) = 0;

		virtual void SerializeToXML(tinyxml2::XMLElement * element) = 0;

		//Parent Functions
		GameObject * GetOwner() {return m_Parent;};
		void SetParent(GameObject * GO);

		int GetOwnerID() {return m_ObjectID;};

		///Get the component's type
		ComponentType GetType() {return m_type;};
		///Set the component's type
		void SetType(ComponentType type) {m_type = type;}; 
		
		void InitializeDataMember(Serializer & Seri, int &DataMember, const char * AttributeName);
		void InitializeDataMember(Serializer & Seri, bool &DataMember, const char * AttributeName);
		void InitializeDataMember(Serializer & Seri, float &DataMember, const char * AttributeName);
		void InitializeDataMember(Serializer & Seri, std::string &DataMember, const char * AttributeName);
        
        void InitializeDataMember(Serializer & Seri, VertexType &DataMember, const char * AttributeName);
		
		template <typename T>
		void WriteDataMember(tinyxml2::XMLElement * element, T &DataMember, const char * AttributeName)
		{
			element->SetAttribute(AttributeName, DataMember); 
		}

		template<>
		void WriteDataMember(tinyxml2::XMLElement * element, std::string &DataMember, const char * AttributeName)
		{
			element->SetAttribute(AttributeName, DataMember.c_str()); 
		}

		template<>
		void WriteDataMember(tinyxml2::XMLElement * element, VertexType &DataMember, const char * AttributeName)
		{
			std::string exportname;
			switch(DataMember)
			{
			case VertexTypeAlbedoModel:
				{
					exportname = "AlbedoModel";
					break;
				}
			case VertexTypeTexturedModel:
				{
					exportname = "TexturedModel";
					break;
				}
			default:
			{
				exportname = "WE'RE GONNA CRASH, BRO!";
			}
			}
			element->SetAttribute(AttributeName, exportname.c_str()); 
		}

	protected:
		GameObject * m_Parent;

	private:
		ComponentType m_type; 
		
		int m_ObjectID;

	};

}