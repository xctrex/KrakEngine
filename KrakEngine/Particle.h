/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation date: 3/27/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "component.h"
#include "LuaInterpreter.h"

namespace KrakEngine
{
	class Particle: public Component{
	public:
		Particle();

		~Particle();

		//accessors
		XMFLOAT3 getCenter(){return m_center;}
		XMFLOAT3 getBoundingValues(){return m_BoundingValues;}
		XMFLOAT3 getMinVelocity(){return m_minVelocity;}
		XMFLOAT3 getMaxVelocity(){return m_maxVelocity;}
		int getMinParticles(){return m_minParticles;}
		int getMaxParticles(){return m_maxParticles;}
		float getMinTime(){return m_minTime;}
		float getMaxTime(){return m_maxTime;}

		bool IsEnabled() { return m_enabled; }

		//mutators
		void particleKilled(){ --m_currentParticles; }
		void decreaseMaxParticle(int decrement);
		void setCenter(XMFLOAT3 center){ 
			m_center = center;
		}
		void setScript(std::string script){m_script = script;}

		void SetEnabled(bool toggle) { m_enabled = toggle; }

		//functions
		void SerializeViaXML(Serializer& Seri);
		void SerializeToXML(tinyxml2::XMLElement * element);
		void Run();

	private:
		XMFLOAT3 m_center;
		XMFLOAT3 m_BoundingValues;
		XMFLOAT3 m_minVelocity;
		XMFLOAT3 m_maxVelocity;
		int m_minParticles;
		int m_maxParticles;
		int m_currentParticles;
		float m_minTime;
		float m_maxTime;

		std::string m_script;

		bool m_enabled;

		LuaInterpreter Li;
	};

};