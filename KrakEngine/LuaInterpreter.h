/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: LuaInterpreter.h
Purpose: Header for LuaInterpreter class
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/7/14
- End Header -----------------------------------------------------*/
#pragma once

//For Lua
#include "Lua\lua.hpp"
#pragma comment(lib, "lua5.1.lib")

#include <string>

namespace KrakEngine
{
	class GameObject;
	class LuaLibrary;

class LuaInterpreter
{

	public:
		LuaInterpreter();
		~LuaInterpreter();

		bool LoadScript(std::string file);
		void RegisterFunction(const char * function, lua_CFunction func);

		void InitializeRegisterFunctions();

		void SetTop();

		static void SetCurrentObj( GameObject * go );
	
	private:
		lua_State * LuaState;

		int CurrentStackPosition;
	
		static LuaLibrary * lualib;
	};

};