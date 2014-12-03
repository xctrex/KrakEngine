/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation date: 3/27/2014
- End Header -----------------------------------------------------*/


#include "Particle.h"
#include "GamePlaySystem.h"
#include <iostream>

namespace KrakEngine
{
	Particle::Particle(): Component(ParticleComp), m_BoundingValues(XMFLOAT3(0,0,0)), m_center(XMFLOAT3(0,0,0)), 
			m_minVelocity(XMFLOAT3(0,0,0)), m_maxVelocity(XMFLOAT3(0,0,0)), m_minTime(0.0f), m_maxTime(0.0f), 
			m_minParticles(0), m_maxParticles(0), m_currentParticles(0), m_enabled(true)
	{
		g_GAMEPLAYSYSTEM->ParticleList.push_back(this);
	};

	void Particle::SerializeViaXML(Serializer& Seri)
	{
		InitializeDataMember(Seri, m_maxTime, "MaxTime");
		InitializeDataMember(Seri, m_minTime, "MinTime");
		InitializeDataMember(Seri, m_maxVelocity.x, "MaxVelocityX");
		InitializeDataMember(Seri, m_maxVelocity.y, "MaxVelocityY");
		InitializeDataMember(Seri, m_maxVelocity.z, "MaxVelocityZ");
		InitializeDataMember(Seri, m_minVelocity.x, "MinVelocityX");
		InitializeDataMember(Seri, m_minVelocity.y, "MinVelocityY");
		InitializeDataMember(Seri, m_minVelocity.z, "MinVelocityZ");
		InitializeDataMember(Seri, m_center.x, "CenterX");
		InitializeDataMember(Seri, m_center.y, "CenterY");
		InitializeDataMember(Seri, m_center.z, "CenterZ");
		InitializeDataMember(Seri, m_BoundingValues.x, "BoundingValuesX");
		InitializeDataMember(Seri, m_BoundingValues.y, "BoundingValuesY");
		InitializeDataMember(Seri, m_BoundingValues.z, "BoundingValuesZ");
		InitializeDataMember(Seri, m_minParticles, "MinParticles");
		InitializeDataMember(Seri, m_maxParticles, "MaxParticles");
		InitializeDataMember(Seri, m_script, "script");
		InitializeDataMember(Seri, m_enabled, "enabled");
	}

	void Particle::SerializeToXML(tinyxml2::XMLElement * element){
		WriteDataMember(element, m_maxTime, "MaxTime");
		WriteDataMember(element, m_minTime, "MinTime");
		WriteDataMember(element, m_maxVelocity.x, "MaxVelocityX");
		WriteDataMember(element, m_maxVelocity.y, "MaxVelocityY");
		WriteDataMember(element, m_maxVelocity.z, "MaxVelocityZ");
		WriteDataMember(element, m_minVelocity.x, "MinVelocityX");
		WriteDataMember(element, m_minVelocity.y, "MinVelocityY");
		WriteDataMember(element, m_minVelocity.z, "MinVelocityZ");
		WriteDataMember(element, m_center.x, "CenterX");
		WriteDataMember(element, m_center.y, "CenterY");
		WriteDataMember(element, m_center.z, "CenterZ");
		WriteDataMember(element, m_BoundingValues.x, "BoundingValuesX");
		WriteDataMember(element, m_BoundingValues.y, "BoundingValuesY");
		WriteDataMember(element, m_BoundingValues.z, "BoundingValuesZ");
		WriteDataMember(element, m_minParticles, "MinParticles");
		WriteDataMember(element, m_maxParticles, "MaxParticles");
		WriteDataMember(element, m_script, "script");
		WriteDataMember(element, m_enabled, "enabled");
	}

	void Particle::Run(){
		if(!m_enabled) return;

		int id = GetOwner()->GetID();
		XMFLOAT3 hi = getCenter();


		if(m_script != ""){
			if(m_currentParticles < m_maxParticles){
				do{
					Li.SetCurrentObj(GetOwner());
					
					Li.LoadScript(m_script);
					m_currentParticles++;
				}while(m_currentParticles < m_minParticles);
			}
		}
	}

	void Particle::decreaseMaxParticle(int decrement){
		m_maxParticles -= decrement;
		if(m_maxParticles < m_minParticles){
			m_minParticles = m_maxParticles;
		}
	}

}
