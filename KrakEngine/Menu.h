/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Menu.h
Purpose: Header for menu.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/25/14
- End Header -----------------------------------------------------*/

#pragma once

#include <vector>
#include <unordered_map>

#include <string>

#include "MenuTypes.h"

namespace KrakEngine{
	
	
class MenuManager;
class GameObject;

	enum ToggleType
	{
		NoToggle,
		OnOff,
		Numbers
	};

	struct Menu_Option
	{
		GameObject * Option;
		GameObject * Toggle;
		int togglevalue;
		ToggleType m_type;

		GameObject * leftarrow;
		GameObject * rightarrow;

		bool operator== (const Menu_Option & rhs)
		{
			return Option == rhs.Option;		
		};
	};

	class GameObject;

	struct MenuCursor
	{
		GameObject * cursor;
		Menu_Option * currentOption;
	};

	class Menu
	{
	public:
		Menu(MenuType type, MenuManager * parent);
		~Menu();

		void SetupMenu();

		void SetOption();

		void SelectCurrentOption();

		void AddOption(std::string optionName);
		void AddOptionWithOnOff(std::string optionName);
		void AddOptionWithNumbers(std::string optionName);
		void Menu::AddOption(std::string optionName, ToggleType type);

		void DisplayImage(std::string imagename, float xsize, float ysize);

		void AddToggleToOption(ToggleType type, std::string optionName);

		typedef void (Menu::*MenuFunction)(Menu_Option &);
		
		//Functions accessed by gameplay
		void MouseClick();	
		void CheckMousePosition();
		void DrawOptions();
		void Up();
		void Down();
		void Left();
		void Right();
		void SelectOption();
		void ExitMenu();
		void ActiveGamepad(bool active){m_gamepad = active;};
		MenuType GetMenuType();

		void LeftMouseRelease() {HasLeftMouseBeenReleased = true;}

	private:
		//useful for confirmation of destructive action.
		//not intended for many layers.

		MenuManager * m_Parent;
	
		MenuType m_curtype;

		GameObject * menutitle;
		std::string m_menuname;

		typedef std::unordered_map<std::string, MenuFunction> MenuFunctionMap;
		MenuFunctionMap menufunction;

		MenuCursor m_MenuCursor;
		GameObject * m_Backdrop;

		GameObject * menuImage;

		bool m_resolutionchange;

		bool m_gamepad;

		typedef  std::vector<Menu_Option> OptionsContainer;
		OptionsContainer m_Options;

		bool IsMouseOverOption(Menu_Option & option);
		bool IsMouseOverLeftButton(Menu_Option & option);
		bool IsMouseOverRightButton(Menu_Option & option);

		void DecrimentToggle();
		void IncrimentToggle();

		float yoffset;

		bool HasLeftMouseBeenReleased;

		Menu_Option * MenuPosition;

		//Initialization
		void InitializeMenu(MenuType type);
		void SetupMenuFunctions();
		void SetupMenuTitle();
		void SetupBackdrop();

		void ResizeBackdrop();

		//On Click functions
		void Restart(Menu_Option &);
		void GoToOptions(Menu_Option &);
		void Quit(Menu_Option &);
		void StartGame(Menu_Option &);
		void BackToGame(Menu_Option &);
		void MainMenu(Menu_Option &);
		void ConfirmYes(Menu_Option &);
		void ConfirmNo(Menu_Option &);
		void Fullscreen(Menu_Option &);
		void DoNothingOnClick(Menu_Option &);
		void Credits(Menu_Option &);
		void HowToPlay(Menu_Option &);
		void ApplyChanges(Menu_Option &);
		void Cancel(Menu_Option &);
		void ReturnFromOption(Menu_Option &);
		void Cheat_NextLevel(Menu_Option &);
		void Cheat_Victory(Menu_Option &);
		void Cheat_Resume(Menu_Option &);
		void Resolution(Menu_Option &);
		void CancelResolution(Menu_Option &);

		void Res_1920X1080(Menu_Option &);
		void Res_1440X900(Menu_Option &);
		void Res_1280X1024(Menu_Option &);
		void Res_1280X800(Menu_Option &);

		void ConfirmRestart(Menu_Option &);
		
		//On Toggle functions
		void Toggle_Fullscreen();
		void Toggle_MusicVolume(int vol);
		void Toggle_FXVolume(int vol);

		std::string GetStartingMusicVolume(Menu_Option & op);
		std::string GetStartingFXVolume(Menu_Option & op);

		bool m_MouseOver;
		bool m_OptionisSelected;
	};

};