/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Deserializer.h
Purpose: Header for Deserializer.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once

#include <string>
#include <fstream>
#include <iostream>

namespace Frankengine{

	class Deserializer{
	public:
		std::ifstream stream;
		///Open the serialization stream from a file.
		bool OpenFile(const std::string& file);
		///Does the stream still have data?
		bool IsGood();
		void ReadInt(int& i);
		void ReadBool(bool& b);
		void ReadFloat(float& f);
		void ReadString(std::string& str);
	};

	//Serialization Operators 


	//Base case of serialization is that the object serializes itself.
	template<typename type>
	inline void StreamRead(Deserializer& stream, type& typeInstance)
	{
		typeInstance.Serialize(stream);
	}

	//Base non recursive stream operators of fundamental types.

	inline void StreamRead(Deserializer& stream, float& f)
	{
		stream.ReadFloat(f);
	}

	inline void StreamRead(Deserializer& stream, int& i)
	{
		stream.ReadInt(i);
	}

	inline void StreamRead(Deserializer& stream, bool& i)
	{
		stream.ReadBool(i);
	}

	inline void StreamRead(Deserializer& stream, std::string& str)
	{
		stream.ReadString(str);
	}
}