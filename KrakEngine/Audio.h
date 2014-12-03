/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 1/17/2014
*/

//#include <xaudio2.h>
#include <string>
#include "serializer.h"
#include "ComponentId.h"
#include "Component.h"
#include "SoundSystem.h"




namespace KrakEngine{
	class Audio : public Component 
	{

	public: 
		Audio();
		~Audio();
		bool playJump();
		bool playAction1();
		bool playAction2();
		void SerializeViaXML(Serializer& stream);
		void SerializeToXML(tinyxml2::XMLElement * element);

		void setJump(XAUDIO2_BUFFER* buffer){ m_jump = buffer;}
		void setAction1(XAUDIO2_BUFFER* buffer){ m_action1 = buffer;}
		void setAction2(XAUDIO2_BUFFER* buffer){ m_action2 = buffer;}

		float getVolume();
		void setVolume(float volume);

		void stop();

		Audio * Next;
		Audio * Prev;

	private:
		std::string m_jumpname;
		std::string m_action1name;
		std::string m_action2name;

		XAUDIO2_BUFFER* m_jump;
		XAUDIO2_BUFFER* m_action1;
		XAUDIO2_BUFFER* m_action2;
		IXAudio2SourceVoice * m_pVoice;

	};
}
