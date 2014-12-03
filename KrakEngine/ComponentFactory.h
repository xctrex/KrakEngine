//componentfactory.h

//a component factory that generates components

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

	class ComponentFactory
	{
	public:
		ComponentFactory();
		~ComponentFactory() {};
		Component * CreateComponent(ComponentType compname);
		Component * CreateComponent(std::string name);
	
		typedef Component * (ComponentFactory::*CompFactoryCreator)();
	
		typedef std::unordered_map<std::string, ComponentType> ComponentByString;
		ComponentByString ComponentStringsMap;

		ComponentType GetComponentTypeFromString(std::string name);
		std::string GetNameFromComponentType(ComponentType ct);

	private:
		typedef std::unordered_map<ComponentType, CompFactoryCreator> CompFactoryFunctionMap;
		CompFactoryFunctionMap CompFactoryFunctions;

		void InitializeComponentFunctionMap();
		void InitializeComponentStringMap();
		
		Component * CreateExample();
		//Component * CreateLife();
		Component * CreateSprite();
		Component * CreateTransform();
		Component * CreatePhysics();
		Component * CreateModel();
        Component * CreateCamera();
        Component * CreateText();
        Component * CreateImage();
		Component * CreateAudio();
		Component * CreateController();
		Component * CreateAction();
		Component * CreateTimer();		
		Component * CreateAI();
		Component * CreateSpawner();
		Component * CreateTrigger();
		Component * CreateParticle();
	};
};