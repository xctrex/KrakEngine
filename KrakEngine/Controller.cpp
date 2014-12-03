/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Serializer.h
Controller component, determines if the object is the object to be controlled.
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/11/2014
- End Header -----------------------------------------------------*/

#include "Controller.h"

namespace KrakEngine
{

		void Controller::SerializeViaXML(Serializer & Seri)
		{
			InitializeDataMember(Seri, player, "player");
			InitializeDataMember(Seri, m_immobile, "immobile");
		};

		void Controller::SerializeToXML(tinyxml2::XMLElement * element)
		{
			WriteDataMember(element, player, "player");
			WriteDataMember(element, m_immobile, "immobile");
		}
		
};