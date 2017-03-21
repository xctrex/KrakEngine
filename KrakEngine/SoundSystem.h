/*

Author: Alexander Kaufmann
Creation Date: 1/17/2014
*/

#pragma once
#include "ISystem.h"
#include "WindowSystem.h"
#include <xaudio2.h>
#include "ObjectLinkedList.h"

#include <iostream>

#define fourccRIFF 'FFIR'
#define fourccDATA 'atad'
#define fourccFMT ' tmf'
#define fourccWAVE 'EVAW'
#define fourccXWMA 'AMWX'
#define fourccDPDS 'sdpd'

namespace KrakEngine{

	class Audio;

	class bufferSpace;

	//Collision flags
	#define	COLLISION_LEFT		0x00000001	//0001
	#define	COLLISION_RIGHT		0x00000002	//0010
	#define	COLLISION_TOP		0x00000004	//0100
	#define	COLLISION_BOTTOM	0x00000008	//1000

	class SoundSystem : public ISystem
	{
	public:
		SoundSystem();
		SoundSystem(WindowSystem* window);
		SoundSystem(const SoundSystem&);
		~SoundSystem();

		bool Initialize();
		void Update(float dt);
		bool Shutdown();
		void HandleMessages(Message* message);

		bool createMusicVoice(IXAudio2SourceVoice** pSourceVoice, WAVEFORMATEXTENSIBLE* wfx);
		bool createMenuVoice(IXAudio2SourceVoice** pSourceVoice, WAVEFORMATEXTENSIBLE* wfx);
		bool createFXVoice(IXAudio2SourceVoice** pSourceVoice, WAVEFORMATEXTENSIBLE* wfx);
		HRESULT createBuffer(LPSTR file, XAUDIO2_BUFFER* buffer, WAVEFORMATEXTENSIBLE* wfx);
		HRESULT createMusicBuffer(LPSTR file, XAUDIO2_BUFFER* buffer, WAVEFORMATEXTENSIBLE* wfx);
		HRESULT createMenuBuffer(LPSTR file, XAUDIO2_BUFFER* buffer, WAVEFORMATEXTENSIBLE* wfx);
		bool setMusic(LPSTR file);
		bool setMusic(XAUDIO2_BUFFER* buffer);
		bool setMenuClick(LPSTR file);
		bool setMenuClick(XAUDIO2_BUFFER* buffer);
		bool playMusic();
		bool stopMusic();
		bool resumeMusic();
		bool setCollision(LPSTR file);
		bool setCollision(XAUDIO2_BUFFER* buffer);
		bool playCollision();
		bool stopCollision();
		bool playMenuClick();
		bool stopMenuClick();
		bool playSound(IXAudio2SourceVoice** pSourceVoice, XAUDIO2_BUFFER** buffer);
		bool stopSound(IXAudio2SourceVoice** pSourceVoice);
		bool resumeSound(IXAudio2SourceVoice** pSourceVoice, XAUDIO2_BUFFER** buffer);
		bool menuVolume();
		bool resumeVolume();
		HRESULT findChunkData(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition);
		HRESULT readChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset);

		bool setMusicVolume(float volume);
		bool setFXVolume(float volume);

		bool setMenuVolume(float volume);
		float getMenuVolume();

		float getMusicVolume();
		float getFXVolume();

		void initializeBuffers(const int& levelNo);
		XAUDIO2_BUFFER* findBuffer(std::string name);
		void clearBuffer();
		void stopAllSound();

		void RemoveAudioComponent(int id);

		WAVEFORMATEXTENSIBLE* getWFX( ){return &m_wfx;}

		ObjectLinkList<Audio> m_AudioObjects;
	private:
		WindowSystem* m_window;
		IXAudio2* m_pXAudio2;
		IXAudio2MasteringVoice* m_pMasterVoice;
		IXAudio2SourceVoice* m_pMusicSource;
		IXAudio2SourceVoice* m_pCollisionSource;
		IXAudio2SourceVoice* m_pMenuSource;
		IXAudio2SubmixVoice * m_pMusicSub;
		IXAudio2SubmixVoice * m_pFXSub;
		IXAudio2SubmixVoice * m_pMenuSub;
		WAVEFORMATEXTENSIBLE m_wfx;
		XAUDIO2_BUFFER* m_music;
		XAUDIO2_BUFFER* m_collision;
		XAUDIO2_BUFFER* m_menuClick;
		UINT32 m_flags;

		std::vector<bufferSpace> buffers;
	};
	extern SoundSystem* g_SOUNDSYSTEM;

	class bufferSpace{
	private:
		XAUDIO2_BUFFER m_buffer;
		std::string m_name;

	public:
		bufferSpace(LPSTR file, std::string name, std::string type): m_name(name){
			ZeroMemory(&m_buffer, sizeof(XAUDIO2_BUFFER));
			if(type == "music"){
				g_SOUNDSYSTEM->createMusicBuffer(file, &m_buffer, g_SOUNDSYSTEM->getWFX());
			}else{
				g_SOUNDSYSTEM->createBuffer(file, &m_buffer, g_SOUNDSYSTEM->getWFX());
			}
		}
		XAUDIO2_BUFFER* getBuffer() {return &m_buffer;}
		std::string* getName() {return &m_name;}
		void deleteBuffer() { delete m_buffer.pAudioData; delete m_buffer.pContext; }
	};

}