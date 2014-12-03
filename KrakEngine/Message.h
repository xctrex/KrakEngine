/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Message.h
Purpose: Messaging Classes for Message System
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once

#include <string>

namespace KrakEngine{
	namespace MId {
		
		enum MessageIdType{
			None,
			Quit,
			KeyPress,
			Debug,
			ObjectDestroyed,
			ClearComponentLists,
			Collision,
			TriggerZone,
			TimerExpired,
			Minimized,
			Maximized,
			Restored,
			Active,		
            ToggleFullScreen,
			ResolutionChanged,
			GamePadActive
		};
	}

	class Message{
	public:
		Message(MId::MessageIdType id) : MessageId(id){};
		MId::MessageIdType MessageId;
		virtual ~Message(){};
	};

	class MessageQuit : public Message{
	public:
		MessageQuit() : Message(MId::Quit) {};
	};

	class KeyMessage : public Message
	{
	public:
		enum PressType{
			isPressed,
			isTriggered
		};
		KeyMessage() : Message(MId::KeyPress) {};	
		
		int Input;
		PressType PressKey;

	};

	class DebugMessage : public Message
	{
	public:
		DebugMessage() : Message(MId::Debug) {};	
	};

	class ObjectDestroyedMessage : public Message
	{
	public:
		ObjectDestroyedMessage(int objid) : id(objid), Message(MId::ObjectDestroyed) {};
		int GetID() {return id;};
	private:
		int id;
	};

	class ClearComponentListsMessage : public Message
	{
	public:
		ClearComponentListsMessage() : Message(MId::ClearComponentLists) {};	
	};

	class CollisionMessage : public Message
	{
	public:
		CollisionMessage(int objId, int collId, int flag) : 
			id(objId), colliderId(collId), collisionFlag(flag), ignore(false), Message(MId::Collision) {};
		~CollisionMessage() {};

		int GetCollisionFlag(){return collisionFlag;};
		int GetObjectID(){return id;};
		int GetColliderID(){return colliderId;};
		bool IsCollisionIgnored() {return ignore;};
		void IgnoreCollision() {ignore = true;};
	private:
		int id;
		int colliderId;
		int collisionFlag;
		bool ignore;
	};
	class TimerExpiredMessage : public Message
	{
	public:
		TimerExpiredMessage(std::string name) : varname(name), Message(MId::TimerExpired) {};
		~TimerExpiredMessage() {};		
		std::string GetVarName() {return varname;};
	private:
		std::string varname;
	};	
	class MinimizeMessage : public Message
	{
	public:
		MinimizeMessage(): Message(MId::Minimized){};
	};

	class MaximizeMessage : public Message
	{
	public:
		MaximizeMessage(): Message(MId::Maximized){};
	};

	class RestoreMessage : public Message
	{
	public:
		RestoreMessage(): Message(MId::Restored){};
	};

	class ActiveMessage : public Message
	{
	public:
		ActiveMessage(bool active): Message(MId::Active), m_active(active) {};
		bool GetActive() {return m_active;}
	private:
		bool m_active;
	};

	class GamePadActiveMessage : public Message
	{
	public:
		GamePadActiveMessage(bool active): Message(MId::GamePadActive), m_active(active){};
		bool IsControllerActive() {return m_active;};
	private:
		bool m_active;
	};

}