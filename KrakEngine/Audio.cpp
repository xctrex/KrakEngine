/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 1/17/2014
*/

#include "Audio.h"

namespace KrakEngine{

	Audio::Audio(): Component(AudioComp), m_pVoice(NULL), m_jump(NULL), m_action1(NULL), m_action2(NULL) {
		g_SOUNDSYSTEM->createFXVoice(&m_pVoice, g_SOUNDSYSTEM->getWFX());
		g_SOUNDSYSTEM->m_AudioObjects.push_back(this);
	}

	Audio::~Audio(){
		m_pVoice->Stop();
		m_pVoice->FlushSourceBuffers();
		m_pVoice->DestroyVoice();
	}

	bool Audio::playJump(){
		if(!m_jump || m_jump->AudioBytes == 0) return false;

		g_SOUNDSYSTEM->playSound(&m_pVoice, &m_jump);	
		return true;
	}

	bool Audio::playAction1(){
		if(!m_action1 || m_action1->AudioBytes == 0) return false;

		g_SOUNDSYSTEM->playSound(&m_pVoice, &m_action1);
		return true;
	}

	bool Audio::playAction2(){
		if(!m_action2 || m_action2->AudioBytes == 0) return false;

		g_SOUNDSYSTEM->playSound(&m_pVoice, &m_action2);
		return true;
	}

	float Audio::getVolume(){
		float volume;
		m_pVoice->GetVolume(&volume);
		return volume;
	}

	void Audio::setVolume(float volume){
		m_pVoice->SetVolume(volume);
	}

	void Audio::SerializeViaXML(Serializer& stream){
		InitializeDataMember(stream, m_jumpname, "jump");
		m_jump = g_SOUNDSYSTEM->findBuffer(m_jumpname);
		InitializeDataMember(stream, m_action1name, "action1");
		m_action1 = g_SOUNDSYSTEM->findBuffer(m_action1name);
		InitializeDataMember(stream, m_action2name, "action2");
		m_action2 = g_SOUNDSYSTEM->findBuffer(m_action2name);
	}

	void Audio::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_jumpname, "jump");
		WriteDataMember(element, m_action1name, "action1");
		WriteDataMember(element, m_action2name, "action2");
	}

	void Audio::stop(){
		m_pVoice->Stop();
	}
}