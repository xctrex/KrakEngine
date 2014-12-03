/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation date: 3/12/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "component.h"
#include "LuaInterpreter.h"
#include "RigidBody.h"

namespace KrakEngine{
	class Trigger : public Component
	{
	public:
		Trigger();
		void SerializeViaXML(Serializer& stream);
		void SerializeToXML(tinyxml2::XMLElement * element);

		XMFLOAT3 GetStartPosition(){return m_StartPos;}
        void SetStartPosition(XMFLOAT3 pos){m_StartPos = pos;}

		XMFLOAT3 GetEndPosition(){return m_EndPos;}
        void SetEndPosition(XMFLOAT3 pos){m_EndPos = pos;}

		XMFLOAT3 GetStartFocus(){return m_StartFocus;}
		void SetStartFocus(XMFLOAT3 pos){m_StartFocus = pos;}

		XMFLOAT3 GetEndFocus(){return m_EndFocus;}
		void SetEndFocus(XMFLOAT3 pos){m_EndFocus = pos;}

		bool GetIsTriggered(){return m_IsTriggered;}
		void SetIsTriggered(bool trigger){m_IsTriggered = trigger;}

		bool GetRun(){return m_Run;}
		void SetRun(bool run){m_Run = run;}

		int GetTriggeredOnButtonPress() { return m_triggeredOnbuttonpress; }
		void SetTriggeredOnButtonPress(int toset) { m_triggeredOnbuttonpress = toset; }

		void Run();

	private:
		XMFLOAT3 m_StartPos;
		XMFLOAT3 m_EndPos;
		XMFLOAT3 m_StartFocus;
		XMFLOAT3 m_EndFocus;
		bool m_IsTriggered;
		bool m_Run;
		bool m_Once;
		
		int m_triggeredOnbuttonpress;

		std::string m_objScriptName;

		RigidBody* objPhysics;

		LuaInterpreter Li;
	};

}