//componentfactory.h

//a component factory that generates components

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------

#include "componentfactory.h"

#include "life.h"
#include "Transform.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Model.h"
#include "Camera.h"
#include "Text.h"
#include "Image.h"
#include "Controller.h"
#include "Audio.h"
#include "Action.h"
#include "Timer.h"
#include "AIComp.h"
#include "Spawner.h"
#include "Trigger.h"
#include "Particle.h"

namespace KrakEngine
{
	ComponentFactory::ComponentFactory() 
	{
		InitializeComponentFunctionMap();

		InitializeComponentStringMap();
	}

	//CreateComponent
	//Finds components by a typeID enum number.
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateComponent(ComponentType name)
	{
		CompFactoryFunctionMap::iterator it = CompFactoryFunctions.find(name);
		if(it != CompFactoryFunctions.end())
			return (this->*it->second)(); 

		return NULL;
	}

	//CreateComponent
	//Finds components by a string name.
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateComponent(std::string name)
	{
		ComponentByString::iterator it = ComponentStringsMap.find(name);
		if(it != ComponentStringsMap.end())
			return CreateComponent(it->second);
		else
			return NULL;
	}
	
	//InitializeComponentFunctionMap
	//
	//--------------------------------------------------------------------------------
	void ComponentFactory::InitializeComponentFunctionMap()
	{
		//CompFactoryFunctions.emplace(LifeComp, &ComponentFactory::CreateLife);
		CompFactoryFunctions.emplace(TransformComp, &ComponentFactory::CreateTransform);
		CompFactoryFunctions.emplace(SpriteComp, &ComponentFactory::CreateSprite);
		CompFactoryFunctions.emplace(RigidBodyComp, &ComponentFactory::CreatePhysics);
		CompFactoryFunctions.emplace(ModelComp, &ComponentFactory::CreateModel);
		CompFactoryFunctions.emplace(CameraComp, &ComponentFactory::CreateCamera);
		CompFactoryFunctions.emplace(TextComp, &ComponentFactory::CreateText);
		CompFactoryFunctions.emplace(ImageComp, &ComponentFactory::CreateImage);
		CompFactoryFunctions.emplace(ControllerComp, &ComponentFactory::CreateController);
		CompFactoryFunctions.emplace(AudioComp, &ComponentFactory::CreateAudio);
		CompFactoryFunctions.emplace(ActionComp, &ComponentFactory::CreateAction);
	//	CompFactoryFunctions.emplace(TimerComp, &ComponentFactory::CreateTimer);		
		CompFactoryFunctions.emplace(AIComp, &ComponentFactory::CreateAI);	
		CompFactoryFunctions.emplace(SpawnerComp, &ComponentFactory::CreateSpawner);
		CompFactoryFunctions.emplace(TriggerComp, &ComponentFactory::CreateTrigger);
		CompFactoryFunctions.emplace(ParticleComp, &ComponentFactory::CreateParticle);
	}

	//InitializeComponentStringMap
	//
	//--------------------------------------------------------------------------------
	void ComponentFactory::InitializeComponentStringMap()
	{
		ComponentStringsMap.emplace("Invalid", InvalidComp);
		ComponentStringsMap.emplace("Transform", TransformComp);
	//	ComponentStringsMap.emplace("Life", LifeComp);
		ComponentStringsMap.emplace("Sprite", SpriteComp);
		ComponentStringsMap.emplace("Animation", AnimationComp);
		ComponentStringsMap.emplace("RigidBody", RigidBodyComp);
		ComponentStringsMap.emplace("Model", ModelComp);
		ComponentStringsMap.emplace("Camera", CameraComp);
		ComponentStringsMap.emplace("Text", TextComp);
		ComponentStringsMap.emplace("Image", ImageComp);
		ComponentStringsMap.emplace("Controller", ControllerComp);
		ComponentStringsMap.emplace("Audio", AudioComp);
		ComponentStringsMap.emplace("Action", ActionComp);
	//	ComponentStringsMap.emplace("Timer", TimerComp);		
		ComponentStringsMap.emplace("AI", AIComp);	
		ComponentStringsMap.emplace("Spawner", SpawnerComp);
		ComponentStringsMap.emplace("Trigger", TriggerComp);
		ComponentStringsMap.emplace("Particle", ParticleComp);
	}

	//GetComponentTypeFromString
	//--------------------------------------------------------------------------------
	ComponentType ComponentFactory::GetComponentTypeFromString(std::string name)
	{
		ComponentByString::iterator it = ComponentStringsMap.find(name);
		if (it == ComponentStringsMap.end())
			return InvalidComp;

		return (*it).second;
	}

	//GetComponentTypeFromString
	//--------------------------------------------------------------------------------
	std::string ComponentFactory::GetNameFromComponentType(ComponentType ct)
	{
		ComponentByString::iterator it = ComponentStringsMap.begin();
		for( ; it != ComponentStringsMap.end(); ++it)
		{
			if(it->second == ct)
				return (it->first);
		}

		return "Invalid";
	}


	//Creates a life component
	//--------------------------------------------------------------------------------
	//Component * ComponentFactory::CreateLife()
	//{
	//	return new Life();
	//}
    
	//Creates a transform component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateTransform()
	{
		return new Transform();
	}

	//Creates a sprite component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateSprite()
	{
		return new Sprite();
	}

	//Creates a physics component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreatePhysics()
	{
		return new RigidBody();
	}

	//Creates a model component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateModel()
	{
		return new Model();
	}

    //Creates a camera component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateCamera()
	{
		return new Camera();
	}

    //Creates a text component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateText()
	{
		return new Text();
	}

    //Creates an image component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateImage()
	{
		return new Image();
	}

	//Creates a controller component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateController()
	{
		return new Controller();
	}

	//Creates an audio component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateAudio()
	{
		return new Audio();
	}

	//Creates an action component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateAction()
	{
		return new Action();
	}
	//Creates a AI component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateAI()
	{
		return new AI();
	}

	Component * ComponentFactory::CreateSpawner()
	{
		return new Spawner();
	}

	//Creates a Trigger component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateTrigger()
	{
		return new Trigger();
	}

	//Creates a Particle component
	//--------------------------------------------------------------------------------
	Component * ComponentFactory::CreateParticle()
	{
		return new Particle();
	}
};