/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or closure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: MenuManager.cpp
Purpose: Controls the menu, abstracts the actuall menus from gameplay.
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 3/30/14
- End Header -----------------------------------------------------*/

#include "MenuManager.h"
#include "Menu.h"

namespace KrakEngine{

	MenuManager::MenuManager() : m_CurrentMenu(NULL),
		m_FullscreenOption(false),
		m_FullscreenSetting(false),
		m_musicvol(0),
		m_fxvol(0),
		m_speedcheat(false),
		m_Resx(0),
		m_Resy(0)
	{
	}

	MenuManager::~MenuManager()
	{
	}

	Menu * MenuManager::CreateNewMenu(MenuType type)
	{
		DeleteCurrentMenu();
		m_CurrentMenu = new Menu(type, this);
		return m_CurrentMenu;
	}

	Menu * MenuManager::CreateNewMenu(MenuType type, MenuType prev)
	{
		DeleteCurrentMenu();
		m_CurrentMenu = new Menu(type, this);
		
		if (prev != Invalid)
		{
			m_prev.push(prev);
		}

		return m_CurrentMenu;
	}

	void MenuManager::DeleteCurrentMenu()
	{
		delete m_CurrentMenu;
		m_CurrentMenu = NULL;
	}

	MenuType MenuManager::GetAndRemoveTop()
	{
		MenuType t = m_prev.top();
		m_prev.pop();
		return t;
	}

	MenuType MenuManager::GetCurrentMenuType()
	{
		if(m_CurrentMenu)
			return m_CurrentMenu->GetMenuType();

		return Invalid;
	}

	void MenuManager::LeftMouseRelease()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->LeftMouseRelease();
	}

	void MenuManager::CheckMousePosition()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->CheckMousePosition();
	}
	
	void MenuManager::DrawOptions()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->DrawOptions();
	}

	void MenuManager::SelectOption()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->SelectOption();
	}

	void MenuManager::Up()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->Up();
	}
	
	void MenuManager::Down()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->Down();
	}
	
	void MenuManager::Left()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->Left();
	}
	
	void MenuManager::Right()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->Right();
	}

	void MenuManager::MouseClick()
	{
		if(m_CurrentMenu)
			m_CurrentMenu->MouseClick();
	}
};