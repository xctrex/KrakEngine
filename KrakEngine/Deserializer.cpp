/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Deserializer.cpp
Purpose: Deserializes file streams into game variables
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "Deserializer.h"

namespace Frankengine{

	bool Deserializer::OpenFile(const std::string& filename){
		stream.open(filename);
		return stream.is_open();
	}

	void Deserializer::ReadFloat(float& f){
		stream >> f;
	}

	void Deserializer::ReadInt(int& i){
		stream >> i;
	}

	void Deserializer::ReadBool(bool& b){
		int i = 0;
		b = false;
		stream >> i;
		if (i == 1)
			b = true;
	}

	void Deserializer::ReadString(std::string& str){
		stream >> str;
	}

	bool Deserializer::IsGood()
	{
		return stream.good();
	}
}