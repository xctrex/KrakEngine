/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: MenuManager.h
Purpose: Header for MenuManager.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 3/30/14
- End Header -----------------------------------------------------*/

#pragma once

#include "MenuTypes.h"
#include <stack>

namespace KrakEngine{


	class Menu;

	class MenuManager
	{
	public:

		MenuManager();
		~MenuManager();

		Menu * CreateNewMenu(MenuType type);
		Menu * CreateNewMenu(MenuType type, MenuType prev);

		void SetFullScreenOption(bool setting) { m_FullscreenOption = setting; }
		bool GetFullScreenOption() { return m_FullscreenOption; }

		void SetFullScreenSetting(bool setting) { m_FullscreenSetting = setting; }
		bool GetFullScreenSetting() { return m_FullscreenSetting; }

		void SetMusicVolOption(int volume) { m_musicvol = volume; }
		int GetMusicVolOption() { return m_musicvol; }

		void SetFXVolOption(int volume) { m_fxvol = volume; }
		int GetFXVolOption() { return m_fxvol; }

		void SetAllSoundSetting(int set) { m_allsound = set; }
		int GetAllSoundSetting() { return m_allsound; }

		void DeleteCurrentMenu();

		MenuType GetAndRemoveTop();

		Menu * GetCurrentMenu() { return m_CurrentMenu; }
		
		void SetResolution(int x, int y)
		{
			m_Resx = x;
			m_Resy = y;
		}

		int GetResX() { return m_Resx; }
		int GetResY() { return m_Resy; }

		//wrappers around menu functions
		MenuType GetCurrentMenuType();
		void LeftMouseRelease();
		void CheckMousePosition();
		void DrawOptions();
		void SelectOption();
		void Up();
		void Down();
		void Left();
		void Right();
		void MouseClick();

	private:

		typedef std::stack<MenuType> Menustack;
		Menustack m_prev;

		Menu * m_CurrentMenu;

		//Toggle info
		bool m_FullscreenOption;
		bool m_FullscreenSetting;
		int m_musicvol;
		int m_fxvol;
		bool m_speedcheat;
		int m_allsound;

		int m_Resx;
		int m_Resy;
	};

};