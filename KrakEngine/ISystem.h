/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: ISystem.h
Purpose: Interface for Systems
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once



#include "Message.h"
#include <string>


namespace KrakEngine{

	class ISystem{
	public:

		virtual void HandleMessages(Message* message)=0;

		virtual void Update(float dt) = 0;

		virtual bool Initialize()=0;
		virtual bool Shutdown()=0;

		virtual ~ISystem(){}
	};
}