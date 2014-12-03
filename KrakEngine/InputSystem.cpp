/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: InputSystem.cpp
Purpose: System for Input Manager
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "InputSystem.h"
#include "Core.h"

#include <iostream>

namespace KrakEngine{

	InputSystem* g_INPUTSYSTEM = NULL;
	InputSystem::InputSystem()
	{
	}

	InputSystem::InputSystem(WindowSystem* window) : m_lefttrigger(NOTPRESSED), m_righttrigger(NOTPRESSED), m_inputController(Keyboard), prevVibration(0)
	{
		m_window = window;
		g_INPUTSYSTEM = this;
	}

	InputSystem::InputSystem(const InputSystem& other)
	{

	}

	InputSystem::~InputSystem()
	{
	}

	void InputSystem::Update(float dt)
	{
		ReadKeyboard();
		ReadMouse();
		//if (!ReadKeyboard() || !ReadMouse())
			//CORE->Broadcast(new ActiveMessage(false));
		
		 if (ReadJoystick()){
			 if (m_inputController == Keyboard){
				 ControllerDetected(true);
			 }
		 }else if (m_inputController == GamePad){
			ControllerDetected(false);
			return;
		}
		ReadTriggers(true);
		ReadTriggers(false);
	}

	void InputSystem::ControllerDetected(bool detected){
		GamePadActiveMessage GAM = GamePadActiveMessage(detected);
		CORE->Broadcast(&GAM);
		if (detected)
			m_inputController = GamePad;
		else
			m_inputController = Keyboard;

	}

	bool InputSystem::IsGamePadActive(){
		if (m_inputController == GamePad)
			return true;
		else
			return false;
	}

	bool InputSystem::Initialize()
	{	
		HRESULT result;
		
		ZeroMemory( &m_state, sizeof(XINPUT_STATE) );
		
		for (int i = 0; i < 256; ++i)
			m_prevKeyState[i] = 0;

		result = DirectInput8Create(m_window->m_hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
		if (FAILED(result))
			return false;

		//bind keyboard
		result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
		if (FAILED(result))
			return false;

		result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
		if(FAILED(result))
		{
			return false;
		}

		result = m_keyboard->SetCooperativeLevel(m_window->m_hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
		if (FAILED(result))
			return false;

		result = m_keyboard->Acquire();

		//bind mouse
		result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
		if (FAILED(result))
			return false;

		result = m_mouse->SetDataFormat(&c_dfDIMouse);
		if(FAILED(result))
			return false;

		result = m_mouse->SetCooperativeLevel(m_window->m_hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		if(FAILED(result))
			return false;

		result = m_mouse->Acquire();
		if (ReadJoystick())
			m_inputController = GamePad;
		else
			m_inputController = Keyboard;
		return true;
	}

	bool InputSystem::Shutdown()
	{
		if (m_keyboard)
		{
			m_keyboard->Unacquire();
			m_keyboard->Release();
			m_keyboard = 0;
		}

		// Release the mouse.
		if(m_mouse){
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
		}

		// Release the main interface to direct input.
		if (m_directInput)
		{
			m_directInput->Release();
			m_directInput = 0;
		}

		return true;
	}

	void InputSystem::HandleMessages(Message *m)
	{
	}

	bool InputSystem::ReadKeyboard()
	{
		HRESULT result;

		// Read the keyboard device.
		for (int i = 0; i < 256; ++i)
			m_prevKeyState[i] = m_keyState[i];

		result = m_keyboard->GetDeviceState(sizeof(m_keyState), (LPVOID)&m_keyState);
		if (FAILED(result))
		{
			// If the keyboard lost focus or was not acquired then try to get control back.
			if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			{
				result = m_keyboard->Acquire();
				if (FAILED(result))
					return false;
			}
			else
			{
				return false;
			}
		}
		return true;
	}

	bool InputSystem::ReadMouse(){
		HRESULT result;

		// Read the mouse device.
		result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
		if(FAILED(result))
		{
			// If the mouse lost focus or was not acquired then try to get control back.
			if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			{
				result = m_mouse->Acquire();
				if (FAILED(result))
					return false;
			}
			else
			{
				return false;
			}
		}else{
			//Reset Position
			m_mouseX = 0;
			m_mouseY = 0;

			// Update the location of the mouse cursor based on the change of the mouse location during the frame.
			m_mouseX = m_mouseState.lX;
			m_mouseY = m_mouseState.lY;

			for(int i = 0; i < 4; ++i){
				m_prevMouseButtons[i] = m_mouseButtons[i];
				if(m_mouseState.rgbButtons[i]){
					m_mouseButtons[i] = true;
				}else{
					m_mouseButtons[i] = false;
				}
			}
		}

		return true;

	}

	void InputSystem::SetVibration(int vibrationVal){
		//if (vibration == vibrationVal)
		//	return;
		vibration = vibrationVal;
		XINPUT_VIBRATION xVibration;
		ZeroMemory( &xVibration, sizeof(XINPUT_VIBRATION) );
		xVibration.wLeftMotorSpeed = vibration; // use any value between 0-65535 here
		xVibration.wRightMotorSpeed = vibration; // use any value between 0-65535 here
		DWORD result = XInputSetState( 0, &xVibration );
	}

	void InputSystem::PauseVibration(){
		prevVibration = vibration;
		SetVibration(0);
	}

	void InputSystem::UnpauseVibration(){
		SetVibration(prevVibration);
	}

	bool InputSystem::ReadJoystick()
	{
		DWORD dwResult;    
		//for (DWORD i=0; i< XUSER_MAX_COUNT; i++ )
		//{
			// Simply get the state of the controller from XInput.
			m_prevState = m_state;
			dwResult = XInputGetState( 0, &m_state );

			if( dwResult == ERROR_SUCCESS ){
				return true;
			}
			else{
				return false; 
			}
		//}
	}

	void InputSystem::ReadTriggers(bool left)
	{
		float pressed;
		TriggerPress * triggerstatus;
		if(left)
		{
			pressed = m_state.Gamepad.bLeftTrigger;
			triggerstatus = &m_lefttrigger;
		}
		else
		{		
			pressed = m_state.Gamepad.bRightTrigger;
			triggerstatus = &m_righttrigger;
		}

		switch (*triggerstatus)
		{
			case NOTPRESSED:
			{
				if(pressed > 0.5f)
					*triggerstatus = JUSTPRESSED;
				break;
			}
			case JUSTPRESSED:
			{
				if(pressed > 0.5f)
					*triggerstatus = HELDDOWN;
				else
					*triggerstatus = NOTPRESSED;
				break;
			}
			case HELDDOWN:
			{
				if(pressed < 0.5f)
					*triggerstatus = NOTPRESSED;
				break;
			}
		}
	}

	bool InputSystem::IsButtonDown(unsigned int button)
	{
		if (m_state.Gamepad.wButtons & button){
			return true;
		}
		return false;
	}

	bool InputSystem::IsLeftThumbStickTriggered(float &X, float &Y)
	{
		if(m_state.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			m_state.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			m_state.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
			m_state.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		{
			m_state.Gamepad.sThumbLX = 0;
			m_state.Gamepad.sThumbLY = 0;
		}
	
		if(m_state.Gamepad.sThumbLX != 0 && m_state.Gamepad.sThumbLY != 0)
		{
			X = m_state.Gamepad.sThumbLX;
			Y = m_state.Gamepad.sThumbLY;
			return true;
		}

		return false;
	}

	bool InputSystem::IsRightThumbStickTriggered(float &X, float &Y)
	{
		if(m_state.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			m_state.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			m_state.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
			m_state.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		{
			m_state.Gamepad.sThumbRX = 0;
			m_state.Gamepad.sThumbRY = 0;
		}
	
		if(m_state.Gamepad.sThumbRX != 0 && m_state.Gamepad.sThumbRY != 0)
		{
			X = m_state.Gamepad.sThumbRX;
			Y = m_state.Gamepad.sThumbRY;
			return true;
		}

		return false;
	}

	bool InputSystem::IsLeftTriggerJustPressed()
	{
		return m_lefttrigger == JUSTPRESSED;
	}

	bool InputSystem::IsRightTriggerJustPressed()
	{
		return m_righttrigger == JUSTPRESSED;
	}
	
	bool InputSystem::IsLeftTriggerDown()
	{
		return m_lefttrigger != NOTPRESSED;
	}

	bool InputSystem::IsRightTriggerDown()
	{
		return m_righttrigger != NOTPRESSED;
	}

	bool InputSystem::IsButtonTriggered(unsigned int button)
	{
		if ( (m_state.dwPacketNumber != m_prevState.dwPacketNumber) &&
			(m_state.Gamepad.wButtons & button) &&
			(! (m_prevState.Gamepad.wButtons & button))){
			return true;
		}
		return false;
	}


	bool InputSystem::IsKeyDown(unsigned int key)
	{
		// Return what state the key is in (pressed/not pressed).
		if (m_keyState[key] & 0x80)
			return true;
		return false;
	}

	bool InputSystem::IsKeyTriggered(unsigned int key)
	{
		if ((m_keyState[key] & 0x80) && (m_prevKeyState[key] == 0))
			return true;
		return false;
	}

	bool InputSystem::HasMouseMoved(){
		if(0 != m_mouseX || 0 != m_mouseY){
			return true;
		}
		return false;
	}

	bool InputSystem::IsMouseButtonDown(MouseButton button){
		return m_mouseButtons[button];
	}

	bool InputSystem::IsMouseButtonTriggered(MouseButton button){
		return m_mouseButtons[button] && m_prevMouseButtons[button] != m_mouseButtons[button];
	}
}