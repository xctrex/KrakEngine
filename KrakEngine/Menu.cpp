/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Menu.cpp
Purpose: Handles the menu for the game
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Jeffrey Kitts, j.kitts
Creation date: 2/25/14
- End Header -----------------------------------------------------*/

#include "Factory.h"
#include "Menu.h"
#include "Transform.h"
#include "GameObject.h"
#include "LuaInterpreter.h"
#include "Action.h"
#include "GameObject.h"
#include "Core.h"
#include "GamePlaySystem.h"
#include "PhysicsSystem.h"
#include "InputSystem.h"
#include "Text.h"
#include "Audio.h"
#include "AISystem.h"

#include <iostream>

#include "Image.h"

#include "MenuManager.h"


namespace KrakEngine{

	Menu::Menu(MenuType type, MenuManager * parent) : yoffset(0.0f), HasLeftMouseBeenReleased(false), m_curtype(type), m_Parent(parent)
	{
		InitializeMenu(type);
	}

	Menu::~Menu()
	{
		//menutitle->Kill();
		//menutitle = NULL;
		m_Backdrop->Kill();

		if(menuImage)
			menuImage->Kill();

		for (OptionsContainer::iterator it = m_Options.begin();
			it != m_Options.end();
			++it)
		{
			(*it).Option->Kill();
			if((*it).Toggle)
				(*it).Toggle->Kill();

			if((*it).leftarrow)
			{
				(*it).leftarrow->Kill();
				(*it).rightarrow->Kill();
			}

		}
		m_Options.clear();

		g_PHYSICSSYSTEM->UnpausePhysics();
		g_AISYSTEM->UnpauseAI();

		g_SOUNDSYSTEM->resumeVolume();
	}

	//SetupMenuFunctions
	//Connects option
	void Menu::SetupMenuFunctions()
	{
		menufunction.emplace("Quit Game", &Menu::Quit);
		menufunction.emplace("Resume Game", &Menu::BackToGame);
		menufunction.emplace("Restart Level", &Menu::Restart);
		menufunction.emplace("Start Game", &Menu::StartGame);
		menufunction.emplace("Main Menu", &Menu::MainMenu);
		menufunction.emplace("Yes", &Menu::ConfirmYes);
		menufunction.emplace("No", &Menu::ConfirmNo);
		menufunction.emplace("Return", &Menu::ReturnFromOption);
		menufunction.emplace("Options", &Menu::GoToOptions);
		menufunction.emplace("Credits", &Menu::Credits);

		menufunction.emplace("How To Play", &Menu::HowToPlay);

		menufunction.emplace("Fullscreen", &Menu::DoNothingOnClick);
		menufunction.emplace("Music Volume", &Menu::DoNothingOnClick);
		menufunction.emplace("FX Volume", &Menu::DoNothingOnClick);

		menufunction.emplace("Go Back", &Menu::ConfirmNo);

		menufunction.emplace("Apply", &Menu::ApplyChanges );

		menufunction.emplace("Accept", &Menu::ConfirmNo );
		menufunction.emplace("Cancel", &Menu::ConfirmNo );
		menufunction.emplace("Back to Options", &Menu::CancelResolution );

		menufunction.emplace("Resolution",  &Menu::Resolution);

		//sound
		menufunction.emplace("Play Music",  &Menu::DoNothingOnClick);
		menufunction.emplace("Play All",  &Menu::DoNothingOnClick);

		//resolution functions
		menufunction.emplace("1920 x 1080",  &Menu::Res_1920X1080);
		menufunction.emplace("1400 x 900",  &Menu::Res_1440X900);
		menufunction.emplace("1280 x 1024",  &Menu::Res_1280X1024);
		menufunction.emplace("1280 x 800",  &Menu::Res_1280X800);
		//cheat menu:
		menufunction.emplace("Next Level",  &Menu::Cheat_NextLevel);
		menufunction.emplace("Speed Up",  &Menu::DoNothingOnClick);
		menufunction.emplace("Double Jump",  &Menu::DoNothingOnClick);
		menufunction.emplace("Victory",  &Menu::Cheat_Victory);	
		menufunction.emplace("Resume", &Menu::Cheat_Resume);

		menufunction.emplace("Restart", &Menu::ConfirmRestart);
	}

	void Menu::MouseClick()
	{
		if(!HasLeftMouseBeenReleased)
			return;

		for (OptionsContainer::iterator optionit = m_Options.begin();
			optionit != m_Options.end();
			++optionit)
		{
			if(IsMouseOverOption(*optionit))
			{
				std::string option = (*optionit).Option->GetType();
				MenuFunctionMap::iterator it = menufunction.begin();
				for( ; it != menufunction.end(); ++it)
				{
					if(it->first == option)
					{
						g_SOUNDSYSTEM->playMenuClick();
						(this->*it->second)(*optionit);
						return;
					}
				}
			}
			else if(IsMouseOverLeftButton(*optionit))
			{
				MenuPosition = &(*optionit);
				DecrimentToggle();
			}
			else if(IsMouseOverRightButton(*optionit))
			{
				MenuPosition = &(*optionit);
				IncrimentToggle();
			}
		}
	}

	//when enter or A is pressed, selects the highlighted option
	void Menu::SelectOption()
	{
		std::string option = (*MenuPosition).Option->GetType();
		MenuFunctionMap::iterator menuit = menufunction.begin();
		for( ; menuit != menufunction.end(); ++menuit)
		{
			if(menuit->first == option)
			{
				g_SOUNDSYSTEM->playMenuClick();
				(this->*menuit->second)(*MenuPosition);
				return;
			}
		}
	}

	MenuType Menu::GetMenuType()
	{
		return m_curtype;
	}

	void Menu::ExitMenu()
	{
		  m_Parent->DeleteCurrentMenu();
	}

	void Menu::AddOption(std::string optionName)
	{
		Menu_Option op;
		op.Option = g_FACTORY->Create("Option");
		op.Option->SetType(optionName);

		Text * tc = op.Option->has(Text);
		tc->SetTextContent(optionName);

		Image * button_image = op.Option->has(Image);

		XMFLOAT3 backpos;
		backpos.x = 0.5f;
		backpos.y = 0.3f;
		backpos.z = 0.0f;

		backpos.y += - yoffset;

		yoffset -= 0.12f;
		backpos.z = 1.0f;

		button_image->SetPosition(backpos);

		XMFLOAT2 buttonsize;

		buttonsize.x = 300;
		buttonsize.y = 60;

		button_image->SetSize(buttonsize);

		//default settings
		op.Toggle = NULL;
		op.leftarrow = NULL;
		op.rightarrow = NULL;

		m_Options.emplace_back(op);
	}

	void Menu::AddToggleToOption(ToggleType type, std::string optionName)
	{
		Menu_Option * option = NULL;
		OptionsContainer::iterator it = m_Options.begin();
		for( ; it != m_Options.end(); ++it )
		{
			if(it->Option->GetType() == optionName)
			{
				option = &(*it);
				break;
			}
		}

		option->Toggle = g_FACTORY->Create("Textbox");

		Text * tc = option->Toggle->has(Text);

		switch (type)
		{
		case OnOff:
			{
				if(option->Option->GetType() == "Fullscreen")
				{
					m_Parent->SetFullScreenOption(g_WINDOWSYSTEM->GetFullScreenMode());

					if(m_Parent->GetFullScreenOption())
					{
						tc->SetTextContent("On");
						option->togglevalue = 1;
					}
					else
					{
						tc->SetTextContent("Off");
						option->togglevalue = 0;
					}
				}			
				else if(option->Option->GetType() == "Speed Up")
				{
					if(g_GAMEPLAYSYSTEM->IsSpedUp())
					{
						tc->SetTextContent("On");
						option->togglevalue = 1;
					}
					else
					{
						tc->SetTextContent("Off");
						option->togglevalue = 0;
					}
				}
					else if(option->Option->GetType() == "Super Jump")
				{
					if(g_GAMEPLAYSYSTEM->IsDoubleJump())
					{
						tc->SetTextContent("On");
						option->togglevalue = 1;
					}
					else
					{
						tc->SetTextContent("Off");
						option->togglevalue = 0;
					}
				}
				else if(option->Option->GetType() == "Play Music")
				{
					if(g_SOUNDSYSTEM->getMusicVolume() > 0)
					{
						tc->SetTextContent("On");
						option->togglevalue = 1;
					}
					else
					{
						tc->SetTextContent("Off");
						option->togglevalue = 0;
					}
				}
				else if(option->Option->GetType() == "Play All")
				{
					int i = g_SOUNDSYSTEM->getFXVolume() * 8;
					m_Parent->SetAllSoundSetting(i);

					if(g_SOUNDSYSTEM->getFXVolume() > 0)
					{
						tc->SetTextContent("On");
						option->togglevalue = 1;
					}
					else
					{
						tc->SetTextContent("Off");
						option->togglevalue = 0;
					}
				}

				break;
			}
		case Numbers:
			{
				if(option->Option->GetType() == "Music Volume")
				{	
					tc->SetTextContent(GetStartingMusicVolume(*option));
				}
				else if(option->Option->GetType() == "FX Volume")
				{
					tc->SetTextContent(GetStartingFXVolume(*option));
				}
				break;
			}
		}

		option->m_type = type;

		option->leftarrow = g_FACTORY->Create("ImageObject");

		Image * leftimage = option->leftarrow->has(Image);
		leftimage->SetImageName("leftarrow");

		option->rightarrow = g_FACTORY->Create("ImageObject");

		Image * rightimage = option->rightarrow->has(Image);
		rightimage->SetImageName("rightarrow");

		XMFLOAT2 size = XMFLOAT2(30, 30);
		leftimage->SetSize(size);
		rightimage->SetSize(size);		
	}

	void Menu::AddOption(std::string optionName, ToggleType type)
	{
		AddOption(optionName);
		AddToggleToOption(type, optionName);
	}

	void Menu::DisplayImage(std::string imagename, float xsize, float ysize)
	{
		menuImage = g_FACTORY->Create("ImageObject");
		Image * ic = menuImage->has(Image);

		ic->SetImageName(imagename);
		ic->SetSize(xsize, ysize);

		XMFLOAT3 imagepos;
		imagepos.x = 0.5f;
		imagepos.y = 0.5f;
		imagepos.z = 0.1f;
		
		yoffset -= 0.6f;

		ic->SetPosition(imagepos);
	}

	void Menu::DrawOptions()
	{
		for (OptionsContainer::iterator it = m_Options.begin();
			it != m_Options.end();
			++it)
		{
			Image * optionimage = (*it).Option->has(Image);

			if(*it == *MenuPosition)
				optionimage->SetImageName("buttonHover");
			else
				optionimage->SetImageName("button");

			if(!m_resolutionchange)
				continue;
			
			XMFLOAT3 imagepos = optionimage->GetPosition();
			XMFLOAT2 imagesize = optionimage->GetSize();

			XMFLOAT3 image_pos2 = imagepos;
			XMFLOAT2 image_size2 = imagesize;
			
			XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();

			imagesize.x = screensize.x * 0.1578947368f;
			imagesize.y = screensize.y * 0.055555555555555f;
			
			imagepos.x -= imagesize.x / (4 * screensize.x);
			imagepos.y -= imagesize.y / (2 * screensize.y);

			Text * t = (*it).Option->has(Text);

			XMFLOAT3 textpos;

			textpos.x = image_pos2.x - ((image_size2.x /screensize.x) / 2) + (60.0f/screensize.x);
			textpos.y = image_pos2.y - ((image_size2.y /screensize.y) / 2);

			t->SetPosition(textpos);

		if(!(*it).Toggle) 
			continue;

			XMFLOAT3 image_pos = optionimage->GetPosition();

			XMFLOAT2 image_size = optionimage->GetSize();
		
			image_pos.x += image_size.x / (2 * screensize.x) + ((screensize.x * 0.185f)/screensize.x);
			image_pos.y -= image_size.y / (2 * screensize.y);

			Text * tc = (*it).Toggle->has(Text);
			tc->SetPosition(image_pos);

			Image * leftimage = (*it).leftarrow->has(Image);

			XMFLOAT3 left_pos = optionimage->GetPosition();
			left_pos.x += image_size.x / (2 * screensize.x) + ((screensize.x * 0.13f)/screensize.x);

			leftimage->SetPosition(left_pos);

			Image * rightimage = (*it).rightarrow->has(Image);

			XMFLOAT3 right_pos = optionimage->GetPosition();
			right_pos.x += image_size.x / (2 * screensize.x) + ((screensize.x * 0.27f)/screensize.x);

			rightimage->SetPosition(right_pos);


		}

		m_resolutionchange = false;
	}

	void Menu::CheckMousePosition()
	{
		m_OptionisSelected = false;

		for (OptionsContainer::iterator it = m_Options.begin();
			it != m_Options.end();
			++it)
		{
			Image * optionimage = ((*it).Option)->has(Image);

			if(IsMouseOverOption(*it))
			{
				m_OptionisSelected = true;

				if(!m_MouseOver)
				{
					g_SOUNDSYSTEM->playMenuClick();
				}
				
				m_MouseOver = true;
				
				optionimage->SetImageName("buttonHover");
				MenuPosition = &(*it);
				return;
			}
			else
				optionimage->SetImageName("button");
		}

			m_MouseOver = false;

	}

	void Menu::Restart(Menu_Option & menu_option)
	{
		MenuType t = m_curtype;

		m_Parent->CreateNewMenu(RestartMenu, t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::GoToOptions(Menu_Option & menu_option)
	{
		MenuType t = m_curtype;
		m_Parent->CreateNewMenu(Options, t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::Quit(Menu_Option & menu_option)
	{
		MenuType t = m_curtype;

		m_Parent->CreateNewMenu(ConfirmToQuit, t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::StartGame(Menu_Option & menu_option)
	{
		m_Parent->DeleteCurrentMenu();
		g_SOUNDSYSTEM->stopAllSound();
		g_SOUNDSYSTEM->clearBuffer();
		g_SOUNDSYSTEM->initializeBuffers(0);
		CORE->SetNextState(GameStates::SceneOne);
	}

	void Menu::Cheat_Resume(Menu_Option & menu_option)
	{
		OptionsContainer::iterator it = m_Options.begin();
		for ( ; it != m_Options.end(); ++it )
		{
			if(it->Option->GetType() == "Speed Up")
				g_GAMEPLAYSYSTEM->SetSpeedUpCheatTo((bool)it->togglevalue);

			if(it->Option->GetType() == "Super Jump")
				g_GAMEPLAYSYSTEM->SetDoubleJumpTo((bool)it->togglevalue);
		}

		m_Parent->DeleteCurrentMenu();
		CORE->SetNextState(GameStates::InGame);
	}

	void Menu::BackToGame(Menu_Option & menu_option)
	{
		m_Parent->DeleteCurrentMenu();
		CORE->SetNextState(GameStates::InGame);
	}

	void Menu::MainMenu(Menu_Option & menu_option)
	{
		MenuType t = m_curtype;

		m_Parent->CreateNewMenu(ConfirmToMain, t);
		
		//I don't know if this is supposed to happen, but I killed sound when returning to main menu in order to keep from crashing.
		//g_SOUNDSYSTEM->stopAllSound();
		//g_SOUNDSYSTEM->clearBuffer();

		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::ConfirmYes(Menu_Option & menu_option)
	{
		if (m_curtype == ConfirmToQuit)
			CORE->SetNextState(GameStates::Quit);		
		else
			CORE->SetNextState(GameStates::InMenu);

		  m_Parent->DeleteCurrentMenu();
	}

	void Menu::ConfirmRestart(Menu_Option & menu_option)
	{
		if (m_curtype == RestartMenu){
			CORE->SetNextState(GameStates::Restart);		
		}else
			CORE->SetNextState(GameStates::InMenu);

		  m_Parent->DeleteCurrentMenu();
	}

	void Menu::CancelResolution(Menu_Option & menu_option)
	{
		XMUINT2 screensize = g_WINDOWSYSTEM->GetCurrentResolution();

		if(screensize.x != m_Parent->GetResX()
			|| screensize.y != m_Parent->GetResY())
		{
			g_WINDOWSYSTEM->ChangeResolution(m_Parent->GetResX(), m_Parent->GetResY());
			Message m(MId::ResolutionChanged);
			CORE->Broadcast(&m);

			ResizeBackdrop();
		}

		MenuType t = m_Parent->GetAndRemoveTop();
		m_Parent->CreateNewMenu(t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::ConfirmNo(Menu_Option & menu_option)
	{
		MenuType t = m_Parent->GetAndRemoveTop();
		m_Parent->CreateNewMenu(t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::Resolution(Menu_Option & menu_option)
	{
		MenuType t = m_curtype;
		m_Parent->CreateNewMenu(ResolutionMenu, t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::Fullscreen(Menu_Option & menu_option)
	{
		Text * tc = menu_option.Toggle->has(Text);
		if(menu_option.togglevalue == 0)
		{
			menu_option.togglevalue = 1;
			tc->SetTextContent("On");
		}
		else
		{
			menu_option.togglevalue = 0;
			tc->SetTextContent("Off");
		}

		m_Parent->SetFullScreenSetting((bool)menu_option.togglevalue);
	}

	void Menu::Credits(Menu_Option & menu_option)
	{	
		MenuType t = m_curtype;

		m_Parent->CreateNewMenu(CreditsMenu, t);
		CORE->SetNextState(GameStates::InMenu);
	}

	
	void Menu::HowToPlay(Menu_Option & menu_option)
	{	
		MenuType t = m_curtype;

		m_Parent->CreateNewMenu(HowToPlayMenu, t);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::ReturnFromOption(Menu_Option & menu_option)
	{
		OptionsContainer::iterator it = m_Options.begin();
		for ( ; it != m_Options.end(); ++it )
		{
			if(it->Option->GetType() == "Fullscreen")
				m_Parent->SetFullScreenSetting((bool)it->togglevalue);
			else if(it->Option->GetType() == "Play Music")
			{
				m_Parent->SetMusicVolOption(it->togglevalue);
			}
			else if(it->Option->GetType() == "Play All")
			{
				m_Parent->SetFXVolOption(it->togglevalue);
			}

			//else if(it->Option->GetType() == "Music Volume")
			//{
			//	m_Parent->SetMusicVolOption(it->togglevalue);
			//}
			//else if(it->Option->GetType() == "FX Volume")
			//{
			//	m_Parent->SetFXVolOption(it->togglevalue);
			//}
		}

		m_Parent->CreateNewMenu(ApplyChangesMenu);
		CORE->SetNextState(GameStates::InMenu);
	}

	void Menu::ApplyChanges(Menu_Option & menu_option)
	{
		if(m_Parent->GetFullScreenOption() != m_Parent->GetFullScreenSetting())
			Toggle_Fullscreen();

		Toggle_MusicVolume(m_Parent->GetMusicVolOption());


		if(m_Parent->GetFXVolOption() != m_Parent->GetAllSoundSetting())
			Toggle_FXVolume(m_Parent->GetFXVolOption());

		MenuType t = m_Parent->GetAndRemoveTop();

		m_Parent->CreateNewMenu(t);
		CORE->SetNextState(GameStates::InMenu);	
	}

	void Menu::Cheat_NextLevel(Menu_Option & menu_option)
	{
		m_Parent->DeleteCurrentMenu();
		if(CORE->GetCurrentLevel() + 1 > MAX_LEVEL)
			CORE->SetNextState(GameStates::VictoryScene);
		else
			CORE->SetNextLevel(CORE->GetCurrentLevel() + 1);CORE->SetNextState(GameStates::InGame);
	}
	
	void Menu::Cheat_Victory(Menu_Option & menu_option)
	{
		m_Parent->DeleteCurrentMenu();
		CORE->SetNextState(GameStates::VictoryScene);
	}

	void Menu::Res_1920X1080(Menu_Option &)
	{
		g_WINDOWSYSTEM->ChangeResolution(1920, 1080);
		Message m(MId::ResolutionChanged);
		CORE->Broadcast(&m);

		ResizeBackdrop();
	}

	void Menu::Res_1440X900(Menu_Option &)
	{
		g_WINDOWSYSTEM->ChangeResolution(1440, 900);
		Message m(MId::ResolutionChanged);
		CORE->Broadcast(&m);

		ResizeBackdrop();
	}

	void Menu::Res_1280X1024(Menu_Option &)
	{
		g_WINDOWSYSTEM->ChangeResolution(1280, 1024);
		Message m(MId::ResolutionChanged);
		CORE->Broadcast(&m);

		ResizeBackdrop();
	}
	
	void Menu::Res_1280X800(Menu_Option &)
	{
		g_WINDOWSYSTEM->ChangeResolution(1280, 800);
		Message m(MId::ResolutionChanged);
		CORE->Broadcast(&m);

		ResizeBackdrop();
	}

	void Menu::DoNothingOnClick(Menu_Option & menu_option)
	{
	}

	bool Menu::IsMouseOverOption(Menu_Option & option)
	{
		//cursor
		Image *ic = m_MenuCursor.cursor->has(Image);
		XMFLOAT3 curspos = ic->GetPosition();

		//option
		Image * option_image = option.Option->has(Image);
		XMFLOAT3 optionpos = option_image->GetPosition();
		XMFLOAT2 optionsize = option_image->GetSize();

		XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();
		optionsize.x /= screensize.x;
		optionsize.y /= screensize.y;

		return (!(curspos.x > optionpos.x + optionsize.x/2.0f
			|| curspos.x < optionpos.x - optionsize.x/2.0f
			|| curspos.y > optionpos.y + optionsize.y/2.0f
			|| curspos.y < optionpos.y - + optionsize.y/2.0f));
	}

	bool Menu::IsMouseOverLeftButton(Menu_Option & option)
	{
		if(!option.leftarrow)
			return false;

		Image *ic = m_MenuCursor.cursor->has(Image);
		XMFLOAT3 curspos = ic->GetPosition();

		//arrow
		Image * option_image = option.leftarrow->has(Image);
		XMFLOAT3 optionpos = option_image->GetPosition();
		XMFLOAT2 optionsize = option_image->GetSize();

		XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();
		optionsize.x /= screensize.x;
		optionsize.y /= screensize.y;

		return (!(curspos.x > optionpos.x + optionsize.x/2.0f
			|| curspos.x < optionpos.x - optionsize.x/2.0f
			|| curspos.y > optionpos.y + optionsize.y/2.0f
			|| curspos.y < optionpos.y - + optionsize.y/2.0f));
	}

	bool Menu::IsMouseOverRightButton(Menu_Option & option)
	{
		if(!option.rightarrow)
			return false;

		Image *ic = m_MenuCursor.cursor->has(Image);
		XMFLOAT3 curspos = ic->GetPosition();

		//arrow
		Image * option_image = option.rightarrow->has(Image);
		XMFLOAT3 optionpos = option_image->GetPosition();
		XMFLOAT2 optionsize = option_image->GetSize();

		XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();
		optionsize.x /= screensize.x;
		optionsize.y /= screensize.y;

		return (!(curspos.x > optionpos.x + optionsize.x/2.0f
			|| curspos.x < optionpos.x - optionsize.x/2.0f
			|| curspos.y > optionpos.y + optionsize.y/2.0f
			|| curspos.y < optionpos.y - + optionsize.y/2.0f));
	}

	void Menu::InitializeMenu(MenuType type)
	{
		g_SOUNDSYSTEM->menuVolume();
		
		m_MenuCursor.cursor = g_FACTORY->GetObjList().FindByName("Cursor");

		menuImage = NULL;
		m_resolutionchange = true;

		switch(type)
		{
		case Start:
			{				
				Image * cursorimage = m_MenuCursor.cursor->has(Image);

				XMFLOAT3 cursorpos = cursorimage->GetPosition();
								
				g_GAMEPLAYSYSTEM->GamePlayLoadMainMenuLevel();

				m_MenuCursor.cursor = g_FACTORY->GetObjList().FindByName("Cursor");

				cursorimage = m_MenuCursor.cursor->has(Image);

				cursorimage->SetPosition(cursorpos);

				m_menuname = "main_menu";

				AddOption("Start Game");
				AddOption("Options");
				AddOption("How To Play");
				AddOption("Credits");
				AddOption("Quit Game");
				break;
			}
		case Pause:
			{
				m_menuname = "Paused";

				yoffset = 0.1f;

				AddOption("Resume Game");
				AddOption("Main Menu");
				AddOption("Options");
				AddOption("How To Play");
				AddOption("Restart Level");
				AddOption("Credits");
				AddOption("Quit Game");
				break;
			}
		case ConfirmToQuit:
			{
				m_menuname = "Are you sure?";

				AddOption("Yes");
				AddOption("No");
				break;
			}
		case ConfirmToMain:
			{
				m_menuname = "Go to main menu?";

				AddOption("Yes");
				AddOption("No");
				break;
			}
		case Options:
			{
				m_menuname = "options";

				XMUINT2 screensize = g_WINDOWSYSTEM->GetCurrentResolution();

				m_Parent->SetResolution(screensize.x, screensize.y);

				AddOption("Fullscreen", OnOff);
				//AddOption("Music Volume", Numbers);
				//AddOption("FX Volume", Numbers);
				AddOption("Play Music", OnOff);
				AddOption("Play All", OnOff);
				AddOption("Resolution");
				AddOption("Return");
				break;
			}
		case CreditsMenu:
			{
				m_menuname = "credits";

				//DisplayImage("credits", 600, 426);

				AddOption("Go Back");
				break;
			}
		case HowToPlayMenu:
			{
				if (g_INPUTSYSTEM->IsGamePadActive())
					m_menuname = "controls";
				else
					m_menuname = "controlsKB";

				//DisplayImage("controls", 450, 320);
				yoffset = -0.6f;
				AddOption("Go Back");
				break;
			}
		case ApplyChangesMenu:
			{
				m_menuname = "Apply Changes?";

				AddOption("Apply");
				AddOption("Cancel");
				break;
			}
		case CheatMenu:
			{
				m_menuname = "Cheat Menu";
				
				AddOption("Speed Up", OnOff);
				AddOption("Super Jump", OnOff);
				AddOption("Next Level");
				AddOption("Victory");
				AddOption("Resume");
				break;
			}
		case RestartMenu:
			{
				m_menuname = "restart";

				AddOption("Restart");
				AddOption("No");
				break;
			}
		case ResolutionMenu:
			{
				m_menuname = "options";

				AddOption("1920 x 1080");
				AddOption("1400 x 900");
				AddOption("1280 x 1024");
				AddOption("1280 x 800");
				AddOption("Accept");
				AddOption("Back to Options");
				break;
			}
		default:
			{
				ThrowErrorIf(true, "Invalid Menu Type");
				break;
			}
		}
		
		m_Backdrop = g_FACTORY->Create("ImageObject");
		SetupBackdrop();
		//SetupMenuTitle();
		SetupMenuFunctions();

		MenuPosition = &(*m_Options.begin());
	}

	void Menu::SetupMenuTitle()
	{
		menutitle = g_FACTORY->Create("Textbox");

		XMFLOAT3 titlepos;
		titlepos.x = 0.42f;
		titlepos.y = 0.05f;
		titlepos.z = 0.0f;

		Text * title_text = menutitle->has(Text);
		title_text->SetTextContent(m_menuname);

		title_text->SetPosition(titlepos);
	}

	void Menu::SetupBackdrop()
	{
		ResizeBackdrop();

		Image * backdrop = m_Backdrop->has(Image);

		backdrop->SetImageName(m_menuname);
		XMFLOAT3 hello = backdrop->GetPosition();
		hello.x = 0.5f;
		hello.y = 0.5f;
		hello.z = -0.1f;

		backdrop->SetPosition(hello);
	}

	void Menu::ResizeBackdrop()
	{
		Image * backdrop = m_Backdrop->has(Image);

		XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();
		XMFLOAT2 size;    
		size.x = (float)screensize.x;
		size.y = (float)screensize.y;

		backdrop->SetSize(size);
	}

	void Menu::Toggle_Fullscreen()
	{
		Message m(MId::ToggleFullScreen);
		CORE->Broadcast(&m); 

		m_resolutionchange = true;

		ResizeBackdrop();
	}

	void Menu::Toggle_MusicVolume(int musicvol)
	{
		g_SOUNDSYSTEM->setMusicVolume((musicvol / 8.0f));
	}

	void Menu::Toggle_FXVolume(int fxvol)
	{
		if(fxvol == 0)
			m_Parent->SetMusicVolOption(0);
		else
			m_Parent->SetMusicVolOption(1);

		g_SOUNDSYSTEM->setMusicVolume((m_Parent->GetMusicVolOption() / 8.0f));
		g_SOUNDSYSTEM->setFXVolume((fxvol / 8.0f));
		g_SOUNDSYSTEM->setMenuVolume((fxvol / 8.0f));
	}

	std::string Menu::GetStartingMusicVolume(Menu_Option & op)
	{
		float f = g_SOUNDSYSTEM->getMusicVolume();
		f *= 8;
		op.togglevalue = (int)(f * 10);

		std::stringstream ss;
		ss<<op.togglevalue;
		return ss.str();
	}

	std::string Menu::GetStartingFXVolume(Menu_Option & op)
	{
		float f = g_SOUNDSYSTEM->getFXVolume();
		f *= 8;
		op.togglevalue = (int)(f * 10);

		std::stringstream ss;
		ss<<op.togglevalue;
		return ss.str();
	}

	void Menu::Up()
	{
		if(*MenuPosition == *(m_Options.begin()))
			return;

		g_SOUNDSYSTEM->playMenuClick();

		OptionsContainer::iterator it = m_Options.begin();
		for ( ; it != m_Options.end(); ++it)
		{
			if ((*it) == *MenuPosition)
			{
				--it;
				MenuPosition = &(*it);
			}
		}
	}

	void Menu::Down()
	{
		if(*MenuPosition == *(m_Options.end() - 1))
			return;

		g_SOUNDSYSTEM->playMenuClick();

		OptionsContainer::iterator it = m_Options.begin();
		for ( ; it != m_Options.end(); ++it)
		{
			if ((*it) == *MenuPosition)
			{
				++it;
				MenuPosition = &(*it);
			}
		}
	}

	void Menu::Left()
	{
		if(!MenuPosition->Toggle) return;

		DecrimentToggle();
	}

	void Menu::Right()
	{
		if(!MenuPosition->Toggle) return;

		IncrimentToggle();
	}
	
	void Menu::DecrimentToggle()
	{
		switch (MenuPosition->m_type)
		{
		case (OnOff):
			{
				if(MenuPosition->togglevalue == 1)
				{
					Text * tc = MenuPosition->Toggle->has(Text);
					MenuPosition->togglevalue = 0;
					tc->SetTextContent("Off");
				}
				break;
			}
		case (Numbers):
			{
				Text * tc = MenuPosition->Toggle->has(Text);

				MenuPosition->togglevalue = max(0, MenuPosition->togglevalue - 1);

				std::stringstream st;
				st<<MenuPosition->togglevalue;

				tc->SetTextContent(st.str());

				break;
			}
		}		
	}

	void Menu::IncrimentToggle()
	{
		switch (MenuPosition->m_type)
		{
		case (OnOff):
			{
				if(MenuPosition->togglevalue == 0)
				{
					Text * tc = MenuPosition->Toggle->has(Text);
					MenuPosition->togglevalue = 1;
					tc->SetTextContent("On");			
				}
				break;
			}
		case (Numbers):
			{
				Text * tc = MenuPosition->Toggle->has(Text);

				MenuPosition->togglevalue = min(10, MenuPosition->togglevalue + 1);

				std::stringstream st;
				st<<MenuPosition->togglevalue;

				tc->SetTextContent(st.str());
				
				break;
			}
		}
	}
};