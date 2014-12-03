/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Transform.h
Purpose: Header for Transform.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "component.h"

namespace KrakEngine{
	class Transform : public Component
	{
	public:
		Transform();
		void SerializeViaXML(Serializer& stream);
		void SerializeToXML(tinyxml2::XMLElement * element);

        XMFLOAT3 GetPosition(){return m_Position;}
        void SetPosition(XMFLOAT3 pos){m_Position = pos;}

        XMFLOAT3 GetRotation(){return m_Rotation;}
        void SetRotation(XMFLOAT3 rot){m_Rotation = rot;}

    private:
		XMFLOAT3 m_Position;
		XMFLOAT3 m_Rotation;
	};
}
