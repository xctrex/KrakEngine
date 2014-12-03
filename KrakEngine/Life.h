//life.h

//a component that measures the object's hp

//Team: Go Ninja
//Project: GAM 541 game
//Author: Jeff Kitts
//--------------------------------------------------------------------------------
#pragma once
#include "component.h"

namespace KrakEngine
{

	class Life :
		public Component
	{
	public:
		Life() : Component(LifeComp) {};
		~Life() {};
		Life(const Life &) {};
		
		virtual void SerializeViaXML(Serializer & Seri);

		//accessors
		int GetMaxHP() {return m_hpMax;};
		int GetCurrentHP() {return m_hpCurrent;};
	
		//mutators
		void SetMaxHP(int maxhp) {m_hpMax = maxhp;};
		void SetCurrentHP(int curhp) {m_hpCurrent = curhp;};

	private:
		int m_hpMax;
		int m_hpCurrent;
	};

};