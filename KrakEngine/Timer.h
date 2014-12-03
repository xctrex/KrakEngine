#pragma once
#include "component.h"
#include "GameObject.h"
#include "Core.h"

#include "GamePlaySystem.h"
#include "Mechanics.h"

#include "Factory.h"

#include "Transform.h"

namespace KrakEngine
{
	class Timer :
		public Component
	{
	public:
		Timer();
		Timer(int time);
		~Timer();
		virtual void SerializeViaXML(Serializer & Seri);
		virtual void SerializeToXML(tinyxml2::XMLElement * element) {};

		virtual void Execute() = 0;
		
		inline int GetTime() {return m_time;}
		inline void SetTime(int time) {m_time = time;}

		void Tickdown();

	private:
		int m_time;
	};

	class DeathTimer :
		public Timer
	{
	public:
		DeathTimer() {};
		DeathTimer(float time) : Timer((int)time) {};
		~DeathTimer();

		void Execute() 
		{
			m_Parent->Kill();	
		};
	};

	class CounterTimer :
		public Timer
	{
	public:
		CounterTimer(float time, std::string varname) : Timer((int)time), name(varname){};
		~CounterTimer() {};

		void Execute() 
		{
			TimerExpiredMessage tem = TimerExpiredMessage(name);
			CORE->Broadcast(&tem);
		};

	private:
		std::string name;
	};

	class MimeBoxTimer :
		public Timer
	{
	public:
		MimeBoxTimer(float time) : Timer((int)time) {};

		void Execute() 
		{
			GameObject * box = g_FACTORY->Create("mimebox2");
			Transform *newbox = box->has(Transform);
			
			Transform *oldbox = m_Parent->has(Transform);

			XMFLOAT3 position = oldbox->GetPosition();
			
			m_Parent->Kill();	

			newbox->SetPosition(position);


			g_GAMEPLAYSYSTEM->GetMechanics().SetGoBox(box);
		};
	};

	class ParticleTimer :
		public Timer
	{
	public:
		ParticleTimer() {};
		ParticleTimer(float time, int id) : Timer((int)time), m_id(id) {};
		~ParticleTimer();

		void Execute() 
		{
			GameObject * go = g_FACTORY->GetObjList().FindByID(m_id);

			if(go)
			{
				Particle* p = go->has(Particle);
				if(p)
					p->particleKilled();
			}
		
				m_Parent->Kill();	
		
			
		};

	private:
		int m_id;
	};

};