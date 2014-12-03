/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: InputSystem.h
Purpose: Header for InputSystem.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "XINPUT9_1_0.lib")
#include "ISystem.h"
#include "WindowSystem.h"

//higher is more sensitive
#define MOUSE_SENSITIVITY 250.0f

#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include "XInput.h"

#include <iostream>

namespace KrakEngine{

	enum TriggerPress
	{
		NOTPRESSED = 0,
		JUSTPRESSED,
		HELDDOWN
	};

	enum MouseButton
	{
		Left = 0,
		Right,
		Middle,
		Other
	};

	enum InputType
	{
		Keyboard,
		GamePad
	};

	class InputSystem : public ISystem
	{
	public:
		InputSystem();
		InputSystem(WindowSystem* window);
		InputSystem(const InputSystem&);
		~InputSystem();

		bool Initialize();
		void Update(float dt);
		bool Shutdown();
		void HandleMessages(Message* message);
		bool IsKeyDown(unsigned int key);
		bool IsKeyTriggered(unsigned int key);
		bool IsButtonDown(unsigned int button);
		bool IsLeftTriggerJustPressed();
		bool IsRightTriggerJustPressed();
		bool IsLeftTriggerDown();
		bool IsRightTriggerDown();
		bool IsButtonTriggered(unsigned int button);
		bool IsLeftThumbStickTriggered(float &Y, float &X);
		bool IsRightThumbStickTriggered(float &Y, float &X);
		bool HasMouseMoved();
		bool IsMouseButtonDown(MouseButton button);
		bool IsMouseButtonTriggered(MouseButton button);
		void SetVibration(int vibrationVal);
		void PauseVibration();
		void UnpauseVibration();
		bool IsGamePadActive();

		float getMouseX() { return (float)m_mouseX;}
		float getMouseY() { return (float)m_mouseY;}

	private:

		bool ReadKeyboard();
		bool ReadMouse();
		bool ReadJoystick();
		void ReadTriggers(bool left);
		void ControllerDetected(bool status);


	private:
		WindowSystem* m_window;
		XINPUT_STATE m_state;
		XINPUT_STATE m_prevState;
		unsigned char m_keyState[256];
		unsigned char m_prevKeyState[256];
		DIMOUSESTATE m_mouseState;
		int m_mouseX, m_mouseY;
		bool m_mouseButtons[4];
		bool m_prevMouseButtons[4];
		IDirectInput8* m_directInput;
		IDirectInputDevice8 *m_keyboard;
		IDirectInputDevice8 *m_joystick;
		IDirectInputDevice8 *m_mouse;
		TriggerPress m_lefttrigger;
		TriggerPress m_righttrigger;
		InputType m_inputController;
		int prevVibration;
		int vibration;
	};
	extern InputSystem* g_INPUTSYSTEM;
		///Message signaling that a key is pressed.
}
