/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Transform.cpp
Purpose: Component for Object Transforms
Language: C++, MSC
Platform: 10.0 , Windows 7StreamRead(stream, 
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "Transform.h"

namespace KrakEngine{
	Transform::Transform() :
        Component(TransformComp),
        m_Position(0.0f, 0.0f, 0.0f),
        m_Rotation(0.0f, 0.0f, 0.0f)
    {}

	void Transform::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, m_Position.x, "PosX");
		InitializeDataMember(stream, m_Position.y, "PosY");
		InitializeDataMember(stream, m_Position.z, "PosZ");
		InitializeDataMember(stream, m_Rotation.x, "RotX");
		InitializeDataMember(stream, m_Rotation.y, "RotY");
		InitializeDataMember(stream, m_Rotation.z, "RotZ");
	}

	void Transform::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_Position.x, "PosX");
		WriteDataMember(element, m_Position.y, "PosY");
		WriteDataMember(element, m_Position.z, "PosZ");
		WriteDataMember(element, m_Rotation.x, "RotX");
		WriteDataMember(element, m_Rotation.y, "RotY");
		WriteDataMember(element, m_Rotation.z, "RotZ");
	}
}