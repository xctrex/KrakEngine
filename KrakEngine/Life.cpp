//life.cpp

//a component that measures the object's hp

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------

#include "life.h"
#include <iostream>

namespace KrakEngine
{
	//SerializeViaXML
	//Tells the serializer how to serialize a life component
	//--------------------------------------------------------------------------------
	void Life::SerializeViaXML(Serializer& Seri)
	{
		InitializeDataMember(Seri, m_hpMax, "HpMax");
		InitializeDataMember(Seri, m_hpCurrent, "HpCurrent");
	}

}
