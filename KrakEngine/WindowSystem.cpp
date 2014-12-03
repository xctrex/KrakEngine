/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: WindowSystem.cpp
Purpose: System for Window Manager
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "WindowSystem.h"
#include "Core.h"
#include "InputSystem.h"
#include <fcntl.h>

#include <iostream>

namespace KrakEngine{

	WindowSystem* g_WINDOWSYSTEM; 

	//std::vector<MsgHandlerStr> registeredHandlers;

	WindowSystem::WindowSystem(){
		g_WINDOWSYSTEM = this;
	}

	WindowSystem::WindowSystem(HINSTANCE hInst,int width,int height,bool full) :
        m_hinstance(hInst),
        m_DefaultScreenWidth(GetSystemMetrics(SM_CXSCREEN) - 200),
        m_DefaultScreenHeight(GetSystemMetrics(SM_CYSCREEN) - 200),
        m_ScreenWidth(width),
        m_ScreenHeight(height),
        m_FullScreen(false)
    {
		g_WINDOWSYSTEM = this;
	}

	WindowSystem::~WindowSystem()
	{
	}
	/*
	void WindowSystem::RegisterHandler(MsgHandler handler, void *pUser)
	{
		MsgHandlerStr inst = {handler,pUser};
		registeredHandlers.push_back(inst);
	}
	
	void WindowSystem::UnregisterHandler(MsgHandler handler, void *pUser)
	{
		MsgHandlerStr inst = {handler,pUser};
		registeredHandlers.push_back(inst);
	}
	*/
	bool WindowSystem::Initialize()
	{
		// Initialize the windows api.
		InitializeWindows();
	
		return true;
	}

	void WindowSystem::Update(float dt)
	{
		MSG msg;
		bool done;


		// Initialize the message structure.
		ZeroMemory(&msg, sizeof(MSG));
	
		// Loop until there is a quit message from the window or the user.
		done = false;
		while(!done)
		{
			// Handle the windows messages.
			if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else{
				done = true;
			}


			// If windows signals to end the application then exit out.
			if(msg.message == WM_QUIT)
			{
				done = true;
                
				CORE->SetNextState(GameStates::Quit);
			}
		}

		if(GetActiveWindow() == m_hwnd || GetFocus() == m_hwnd){
			if(!m_active){
				CORE->Broadcast(new ActiveMessage(true));
			}
			m_active = true;
			// Confine Cursor to the window
			RECT R;
			GetWindowRect(m_hwnd, &R);
		
			ClipCursor(&R);
		}else{
			if(m_active){
				CORE->Broadcast(new ActiveMessage(false));
			}
			m_active = false;
			ClipCursor(NULL);
		}

		return;
	}
	
	void WindowSystem::ChangeResolution(int width, int height)
	{
		m_Resx = width;
		m_Resy = height;
		DEVMODE dev;

		ZeroMemory(&dev, sizeof(dev));
		dev.dmSize = sizeof(dev);

		dev.dmPelsWidth = width;
		dev.dmPelsHeight = height;
		
		dev.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT;
		m_DefaultScreenWidth = width - 20;
		m_DefaultScreenHeight = height - 20;
		ChangeDisplaySettings(&dev, 0);
		  m_FullScreen = !m_FullScreen;

		  ToggleFullScreen();

	}

    void WindowSystem::GetDesktopResolution(int &x, int &y){
        RECT desktop;
        const HWND hDesktop = GetDesktopWindow();
        GetWindowRect(hDesktop, &desktop);
        x = desktop.right;
        y = desktop.bottom;
    }

    void WindowSystem::ToggleFullScreen(){
        if(!m_FullScreen){
            // Get the size of the full screen
            //m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
            //m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
            GetDesktopResolution(m_ScreenWidth, m_ScreenHeight);

            SetWindowLong(m_hwnd, GWL_STYLE, WS_POPUPWINDOW);
            SetWindowPos(m_hwnd, HWND_TOP,
                0,
                0,
                m_ScreenWidth,
                m_ScreenHeight,
                SWP_SHOWWINDOW);
        }
        else{
            // Calculate the adjusted screen width
            RECT R = { 0, 0, m_DefaultScreenWidth, m_DefaultScreenHeight };
			//RECT R = { 0, 0, m_ScreenWidth, m_ScreenHeight };
		    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
            m_ScreenWidth  = R.right - R.left;
		    m_ScreenHeight = R.bottom - R.top;
            
            SetWindowLong(m_hwnd, GWL_STYLE, (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
            SetWindowPos(m_hwnd, HWND_NOTOPMOST,
                0,
                0,
                m_ScreenWidth,
                m_ScreenHeight,
                SWP_SHOWWINDOW);
            
        }
        m_FullScreen = !m_FullScreen;
            
    }

	void WindowSystem::HandleMessages(Message* message){
        switch(message->MessageId)
        {
            case MId::ToggleFullScreen :
            {
                ToggleFullScreen();
				break;
            }
			case MId::Active :
			{
				ActiveMessage * ODM = dynamic_cast<ActiveMessage *>(message);
				if(!ODM->GetActive()){
					ShowWindow(m_hwnd,SW_MINIMIZE);
				}
				break;
			
			}
		}
	}

	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
	{
		 //if( TwEventWin(hwnd, umsg, wparam, lparam) ) // send event message to AntTweakBar
	       // return 0;

		switch(umsg)
		{
			case WM_SIZE:
			{
				if(wparam == SIZE_MINIMIZED){
					CORE->Broadcast(new MinimizeMessage());
					std::cout << "MINIMIZED" << std::endl;
				}else if(wparam == SIZE_RESTORED){
					CORE->Broadcast(new RestoreMessage());
					std::cout << "RESTORED" << std::endl;
				}else if(wparam == SIZE_MAXIMIZED){
                    CORE->Broadcast(new MaximizeMessage());
					std::cout << "MAXIMIZED" << std::endl;
				}
				return 0;
			}
            case WM_DISPLAYCHANGE:
            {
                Message m(MId::ResolutionChanged);
                CORE->Broadcast(&m);
                return 0;
            }
			// Check if the window is being destroyed.
			case WM_DESTROY:
			{
				PostQuitMessage(0);
				return 0;
			}

			// Check if the window is being closed.
			case WM_CLOSE:
			{
				PostQuitMessage(0);		
				return 0;
			}
			// Any other messages send to the default message handler as our application won't make use of them.
			default:
			{
				return DefWindowProc(hwnd, umsg, wparam, lparam);
			}
		}
	}

	bool WindowSystem::InitializeWindows()
	{
		WNDCLASS wc;
		wc.style         = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc   = MessageHandler; 
		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hInstance     = m_hinstance;
		wc.hIcon         = LoadIcon(0, IDI_APPLICATION);
		wc.hCursor       = LoadCursor(0, IDC_ARROW);
		wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
		wc.lpszMenuName  = 0;
		wc.lpszClassName = "D3DWndClassName";

		if( !RegisterClass(&wc) )
		{
			MessageBox(0, "RegisterClass Failed.", 0, 0);
			return false;
		}

		
#if defined(_DEBUG)
        // Compute window rectangle dimensions based on requested client area dimensions.
		RECT R = { 0, 0, m_DefaultScreenWidth, m_DefaultScreenHeight };
		AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
		m_ScreenWidth  = R.right - R.left;
		m_ScreenHeight = R.bottom - R.top;

        m_FullScreen = false;
		m_hwnd = CreateWindow("D3DWndClassName", "Hat-Life", 
			(WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX), 0, 0, m_ScreenWidth, m_ScreenHeight, 0, 0, m_hinstance, 0); 
		if( !m_hwnd )
		{
			MessageBox(0, "CreateWindow Failed.", 0, 0);
			return false;
		}
		ShowWindow(m_hwnd, SW_SHOW);
#else
        // Get the size of the full screen
		//m_ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
        //m_ScreenHeight = GetSystemMetrics(SM_CYSCREEN);
        GetDesktopResolution(m_ScreenWidth, m_ScreenHeight);

        m_FullScreen = true;
		m_hwnd = CreateWindow("D3DWndClassName", "Hat-Life", 
            WS_POPUP, 0, 0, m_ScreenWidth, m_ScreenHeight, 0, 0, m_hinstance, 0); 
		if( !m_hwnd )
		{
			MessageBox(0, "CreateWindow Failed.", 0, 0);
			return false;
		}

        m_FullScreen = false;
        ToggleFullScreen();

		ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
#endif

		//m_Resx = GetSystemMetrics(SM_CXSCREEN);
		//m_Resy = GetSystemMetrics(SM_CYSCREEN);
        GetDesktopResolution(m_Resx, m_Resy);

		UpdateWindow(m_hwnd);
		ShowCursor(false);
		SetCursorPos(m_ScreenWidth/2, m_ScreenHeight/2);
		m_active = true;
		return true;
	}

	void WindowSystem::ShowTheCursor(bool toggle)
	{
		ShowCursor(toggle);
	}

	bool WindowSystem::Shutdown()
	{
		// Show the mouse cursor.
		ShowCursor(true);

		// Fix the display settings if leaving full screen mode.

		// Remove the window.
		DestroyWindow(m_hwnd);
		m_hwnd = NULL;

		// Remove the application instance.
		UnregisterClass(m_applicationName, m_hinstance);
		m_hinstance = NULL;

		// Release the pointer to this class.
		g_WINDOWSYSTEM = NULL;
		
		return true;
	}

	//bool WindowSystem::IsEscDown(){ return GetKeyState( VK_ESCAPE ) < 0; }
}