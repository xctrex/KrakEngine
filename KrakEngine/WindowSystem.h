/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: WindowSystem.h
Purpose: Header for WindowSystem.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/


#pragma once

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define WIN32_LEAN_AND_MEAN



//////////////
// INCLUDES //
//////////////
#include <windows.h>  //PRECOMPILE

#include "ISystem.h"



////////////////////////////////////////////////////////////////////////////////
// Class name: SystemClass
////////////////////////////////////////////////////////////////////////////////
namespace KrakEngine{

	typedef LRESULT (__stdcall *MsgHandler)(HWND, UINT, WPARAM, LPARAM, void*);

	struct MsgHandlerStr{
		MsgHandler fn;
		void *pUser;
	};

	class WindowSystem : public ISystem
	{

	public:

		WindowSystem();
		WindowSystem(HINSTANCE hInst,int height,int width,bool full);
		WindowSystem(const WindowSystem&);
		~WindowSystem();

		bool Initialize();
		bool Shutdown();
		void Update(float dt);
		void HandleMessages(Message* message);

		void ChangeResolution(int x, int y);

        void ToggleFullScreen();
        bool GetFullScreenMode()const { return m_FullScreen; }
        void GetDesktopResolution(int &x, int &y);
		void ShowTheCursor(bool toggle);

		XMUINT2 GetCurrentResolution() { return XMUINT2(m_Resx, m_Resy); }

	private:
		bool InitializeWindows();
		void ShutdownWindows();
		//bool IsEscDown();
		//void RegisterHandler(MsgHandler handler, void *pUser);
		//void UnregisterHandler(MsgHandler handler, void *pUser);

	public:
		HWND m_hwnd;
		HINSTANCE m_hinstance;
		int m_ScreenWidth, m_ScreenHeight;
        int m_DefaultScreenWidth, m_DefaultScreenHeight;
		int m_Resx, m_Resy;
		bool m_FullScreen;

	private:
		LPCSTR m_applicationName;
		std::wstring mMainWndCaption;
		bool m_active;
		
	};

	static LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

	extern WindowSystem* g_WINDOWSYSTEM;
}