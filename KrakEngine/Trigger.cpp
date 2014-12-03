/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation date: 3/12/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "Trigger.h"
#include "GameObject.h"
#include "GamePlaySystem.h"
#include "Factory.h"
#include "ControllerSystem.h"

namespace KrakEngine{

	Trigger::Trigger() : Component(TriggerComp), m_StartPos(XMFLOAT3(0,0,0)), m_EndPos(XMFLOAT3(0,0,0)), 
						m_StartFocus(XMFLOAT3(0,0,0)), m_EndFocus(XMFLOAT3(0,0,0)), m_IsTriggered(false), 
						m_objScriptName(""), m_Run(false), m_Once(true), m_triggeredOnbuttonpress(0)
	{
		g_GAMEPLAYSYSTEM->TriggerList.emplace_back(this);	
	}

	void Trigger::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, m_objScriptName, "Script");
		InitializeDataMember(stream, m_StartPos.x, "StartPosX");
		InitializeDataMember(stream, m_StartPos.y, "StartPosY");
		InitializeDataMember(stream, m_StartPos.z, "StartPosZ");
		InitializeDataMember(stream, m_EndPos.x, "EndPosX");
		InitializeDataMember(stream, m_EndPos.y, "EndPosY");
		InitializeDataMember(stream, m_EndPos.z, "EndPosZ");
		InitializeDataMember(stream, m_StartFocus.x, "StartFocusX");
		InitializeDataMember(stream, m_StartFocus.y, "StartFocusY");
		InitializeDataMember(stream, m_StartFocus.z, "StartFocusZ");
		InitializeDataMember(stream, m_EndFocus.x, "EndFocusX");
		InitializeDataMember(stream, m_EndFocus.y, "EndFocusY");
		InitializeDataMember(stream, m_EndFocus.z, "EndFocusZ");
		InitializeDataMember(stream, m_IsTriggered, "Triggered");
		InitializeDataMember(stream, m_Once, "Once");
		InitializeDataMember(stream, m_triggeredOnbuttonpress, "Button");
	}

	void Trigger::SerializeToXML(tinyxml2::XMLElement * element){
		WriteDataMember(element, m_objScriptName, "Script");
		WriteDataMember(element, m_StartPos.x, "StartPosX");
		WriteDataMember(element, m_StartPos.y, "StartPosY");
		WriteDataMember(element, m_StartPos.z, "StartPosZ");
		WriteDataMember(element, m_EndPos.x, "EndPosX");
		WriteDataMember(element, m_EndPos.y, "EndPosY");
		WriteDataMember(element, m_EndPos.z, "EndPosZ");
		WriteDataMember(element, m_StartFocus.x, "StartFocusX");
		WriteDataMember(element, m_StartFocus.y, "StartFocusY");
		WriteDataMember(element, m_StartFocus.z, "StartFocusZ");
		WriteDataMember(element, m_EndFocus.x, "EndFocusX");
		WriteDataMember(element, m_EndFocus.y, "EndFocusY");
		WriteDataMember(element, m_EndFocus.z, "EndFocusZ");
		WriteDataMember(element, m_IsTriggered, "Triggered");
		WriteDataMember(element, m_Once, "Once");
		WriteDataMember(element, m_triggeredOnbuttonpress, "Button");
	}

	void Trigger::Run(){

		if(m_triggeredOnbuttonpress == 1) return;

		GameObject* obj = g_CONTROLLERSYSTEM->FindController();
		objPhysics = obj->has(RigidBody);

		if(objPhysics){
			m_IsTriggered = (
				objPhysics->position.x >= m_StartPos.x && objPhysics->position.x <= m_EndPos.x &&
				objPhysics->position.y >= m_StartPos.y && objPhysics->position.y <= m_EndPos.y
				) & !m_Run;

			if(m_IsTriggered){
				m_Run = true;
				Li.SetCurrentObj(GetOwner());
				Li.LoadScript(m_objScriptName);
				if(m_Once){
					m_objScriptName = "";
				}
			}

			if((objPhysics->position.x <= m_StartPos.x || objPhysics->position.x >= m_EndPos.x)){
				m_Run = false;
			}
		}
	}

}