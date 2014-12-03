/*
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
Author: Alexander Kaufmann
Creation Date: 1/17/2014
*/

#include "Precompiled.h"
#include "SoundSystem.h"
#include "serializer.h"
#include "Core.h"
#include "ControllerSystem.h"
#include "GameObject.h"
#include "Audio.h"

#include <iostream>

namespace KrakEngine{

		SoundSystem* g_SOUNDSYSTEM = NULL;

		SoundSystem::SoundSystem()
		{
			g_SOUNDSYSTEM = this;
		}

		SoundSystem::SoundSystem(WindowSystem* window)
		{
			m_window = window;
			g_SOUNDSYSTEM = this;
		}

		SoundSystem::SoundSystem(const SoundSystem&)
		{
		}

		SoundSystem::~SoundSystem()
		{
		}

		bool SoundSystem::Initialize()
		{
			#if defined(_DEBUG)
				m_flags = XAUDIO2_DEBUG_ENGINE;
			#else
				m_flags = 0;
			#endif
			
				
			m_pXAudio2 = NULL;
			DXThrowIfFailed(XAudio2Create( &m_pXAudio2, m_flags, XAUDIO2_DEFAULT_PROCESSOR ) );

			m_pMasterVoice = NULL;
			DXThrowIfFailed(m_pXAudio2->CreateMasteringVoice( &m_pMasterVoice ) );

			m_pXAudio2->CreateSubmixVoice(&m_pMusicSub, 1, 44100, 0, 0, 0, 0);
			m_pXAudio2->CreateSubmixVoice(&m_pFXSub, 1, 44100, 0, 0, 0, 0);
			m_pXAudio2->CreateSubmixVoice(&m_pMenuSub, 1, 44100, 0, 0, 0, 0);

			buffers = std::vector<bufferSpace>();

			initializeBuffers(CORE->GetCurrentLevel());

			return true;

		}

		void SoundSystem::Update(float dt)
		{
		}

		bool SoundSystem::Shutdown()
		{
			m_pMusicSource->Stop();
			m_pMusicSource->FlushSourceBuffers();
			m_pMusicSource->DestroyVoice();

			delete m_music;

			return true;
		}
		void SoundSystem::HandleMessages(Message* message){
			if(message->MessageId == MId::ObjectDestroyed){
				ObjectDestroyedMessage * ODM = dynamic_cast<ObjectDestroyedMessage *>(message);
				RemoveAudioComponent(ODM->GetID());
			}else if(message->MessageId == MId::ClearComponentLists){
				ObjectLinkList<Audio>::iterator it = m_AudioObjects.begin();
				for(;it !=m_AudioObjects.end();++it){
					it->stop();
					m_AudioObjects.erase(it);
				}
			}else if(message->MessageId == MId::Collision){
				CollisionMessage * ODM = dynamic_cast<CollisionMessage *>(message);
				GameObject* temp = g_CONTROLLERSYSTEM->FindController();
				if(!temp)
					return;
				if((ODM->GetObjectID() == temp->GetID())){
					if(ODM->GetCollisionFlag() & COLLISION_LEFT || ODM->GetCollisionFlag() & COLLISION_RIGHT){
						playCollision();
					}
				}
			}else if(message->MessageId == MId::Minimized){
				stopMusic();
			}else if(message->MessageId == MId::Restored){
				resumeMusic();
			}else if(message->MessageId == MId::Active){
				ActiveMessage * ODM = dynamic_cast<ActiveMessage *>(message);
				if(ODM->GetActive()){
					resumeMusic();
				}else{
					stopMusic();
				}
			}
		}

		bool SoundSystem::createMusicVoice(IXAudio2SourceVoice** pSourceVoice, WAVEFORMATEXTENSIBLE* wfx)
		{
			XAUDIO2_SEND_DESCRIPTOR SFXSend = {0, m_pMusicSub};
			XAUDIO2_VOICE_SENDS SFXSendList = {1, &SFXSend};

			DXThrowIfFailed(m_pXAudio2->CreateSourceVoice( *&pSourceVoice, (WAVEFORMATEX*)wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, &SFXSendList, NULL ) ); 
			
			return true;
		}

		bool SoundSystem::createMenuVoice(IXAudio2SourceVoice** pSourceVoice, WAVEFORMATEXTENSIBLE* wfx)
		{
			XAUDIO2_SEND_DESCRIPTOR SFXSend = {0, m_pMenuSub};
			XAUDIO2_VOICE_SENDS SFXSendList = {1, &SFXSend};

			DXThrowIfFailed(m_pXAudio2->CreateSourceVoice( *&pSourceVoice, (WAVEFORMATEX*)wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, &SFXSendList, NULL ) ); 
			
			return true;
		}

		bool SoundSystem::createFXVoice(IXAudio2SourceVoice** pSourceVoice, WAVEFORMATEXTENSIBLE* wfx)
		{
			XAUDIO2_SEND_DESCRIPTOR SFXSend = {0, m_pFXSub};
			XAUDIO2_VOICE_SENDS SFXSendList = {1, &SFXSend};

			DXThrowIfFailed(m_pXAudio2->CreateSourceVoice( *&pSourceVoice, (WAVEFORMATEX*)wfx, 0, XAUDIO2_DEFAULT_FREQ_RATIO, 0, &SFXSendList, NULL ) ); 
			
			return true;
		}

		HRESULT SoundSystem::createBuffer(LPSTR file, XAUDIO2_BUFFER* buffer, WAVEFORMATEXTENSIBLE* wfx)
		{
			HANDLE hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

			
			if( INVALID_HANDLE_VALUE == hFile )
				return HRESULT_FROM_WIN32( GetLastError() );
			
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
				return HRESULT_FROM_WIN32( GetLastError() );
			
			DWORD dwChunkSize;
			DWORD dwChunkPosition;
			//check the file type, should be fourccWAVE or 'XWMA'
			findChunkData(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
			DWORD filetype;

			readChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition);
			if (filetype != fourccWAVE)
				return S_FALSE;

			findChunkData(hFile,fourccFMT, dwChunkSize, dwChunkPosition );
			readChunkData(hFile, wfx, dwChunkSize, dwChunkPosition );

			//fill out the audio data buffer with the contents of the fourccDATA chunk
			findChunkData(hFile,fourccDATA,dwChunkSize, dwChunkPosition );
			BYTE * pDataBuffer = new BYTE[dwChunkSize];
			readChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);

			buffer->AudioBytes = dwChunkSize;  //buffer containing audio data
			buffer->pAudioData = pDataBuffer;  //size of the audio buffer in bytes
			buffer->LoopCount = 0;			   //Don't Loop
			buffer->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

			return S_OK;
		}

		HRESULT SoundSystem::createMusicBuffer(LPSTR file, XAUDIO2_BUFFER* buffer, WAVEFORMATEXTENSIBLE* wfx)
		{
			HANDLE hFile = CreateFile(file, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL );

			
			if( INVALID_HANDLE_VALUE == hFile )
				return HRESULT_FROM_WIN32( GetLastError() );
			
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
				return HRESULT_FROM_WIN32( GetLastError() );
			
			DWORD dwChunkSize;
			DWORD dwChunkPosition;
			//check the file type, should be fourccWAVE or 'XWMA'
			findChunkData(hFile,fourccRIFF,dwChunkSize, dwChunkPosition );
			DWORD filetype;

			DXThrowIfFailed(readChunkData(hFile,&filetype,sizeof(DWORD),dwChunkPosition));
			if (filetype != fourccWAVE)
				return S_FALSE;

			DXThrowIfFailed(findChunkData(hFile,fourccFMT, dwChunkSize, dwChunkPosition ));
			DXThrowIfFailed(readChunkData(hFile, wfx, dwChunkSize, dwChunkPosition ));

			//fill out the audio data buffer with the contents of the fourccDATA chunk
			DXThrowIfFailed(findChunkData(hFile,fourccDATA,dwChunkSize, dwChunkPosition ));
			BYTE * pDataBuffer = new BYTE[dwChunkSize];
			DXThrowIfFailed(readChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition));

			buffer->AudioBytes = dwChunkSize;  //buffer containing audio data
			buffer->pAudioData = pDataBuffer;  //size of the audio buffer in bytes
			buffer->LoopCount = XAUDIO2_LOOP_INFINITE;
			buffer->Flags = XAUDIO2_END_OF_STREAM; // tell the source voice not to expect any data after this buffer

			return S_OK;
		}

		bool SoundSystem::setMusic(LPSTR file){
			DXThrowIfFailed(createMusicBuffer(file, m_music, &m_wfx));
			createMusicVoice(&m_pMusicSource, &m_wfx);
			return true;
		}

		bool SoundSystem::setMusic(XAUDIO2_BUFFER* buffer){
			m_music = buffer;
			createMusicVoice(&m_pMusicSource, &m_wfx);
			return true;
		}

		bool SoundSystem::setMenuClick(LPSTR file){
			DXThrowIfFailed(createBuffer(file, m_menuClick, &m_wfx));
			createMenuVoice(&m_pMenuSource, &m_wfx);
			return true;
		}

		bool SoundSystem::setMenuClick(XAUDIO2_BUFFER* buffer){
			m_menuClick = buffer;
			createMenuVoice(&m_pMenuSource, &m_wfx);
			return true;
		}

		bool SoundSystem::playMusic()
		{
			return playSound(&m_pMusicSource, &m_music);
		}

		bool SoundSystem::stopMusic()
		{
			return stopSound(&m_pMusicSource);
			
		}

		bool SoundSystem::playMenuClick()
		{
			return playSound(&m_pMenuSource, &m_menuClick);
		}

		bool SoundSystem::stopMenuClick()
		{
			return stopSound(&m_pMenuSource);
			
		}

		bool SoundSystem::resumeMusic()
		{
			return resumeSound(&m_pMusicSource, &m_music);
			
		}

		
		bool SoundSystem::setCollision(LPSTR file){
			DXThrowIfFailed(createBuffer(file, m_collision, &m_wfx));
			createFXVoice(&m_pCollisionSource, &m_wfx);
			return true;
		}

		bool SoundSystem::setCollision(XAUDIO2_BUFFER* buffer){
			m_collision = buffer;
			createFXVoice(&m_pCollisionSource, &m_wfx);
			return true;
		}

		bool SoundSystem::playCollision()
		{
			if(m_collision){
				XAUDIO2_VOICE_STATE state;
				m_pCollisionSource->GetState(&state);
				if(!state.BuffersQueued){
					m_pCollisionSource->SubmitSourceBuffer(m_collision);
					DXThrowIfFailed(m_pCollisionSource->Start( 0 ) );
				}		
			}

			return true;
		}
		bool SoundSystem::stopCollision()
		{
			return stopSound(&m_pCollisionSource);
			
		}
		bool SoundSystem::playSound(IXAudio2SourceVoice** pSourceVoice, XAUDIO2_BUFFER** buffer)
		{
			if(*buffer){
				XAUDIO2_VOICE_STATE state;
				pSourceVoice[0]->GetState(&state);
				if(!state.BuffersQueued){
					pSourceVoice[0]->SubmitSourceBuffer(*buffer);
					DXThrowIfFailed(pSourceVoice[0]->Start( 0 ) );
				}
			}

			return true;
		}

		bool SoundSystem::resumeSound(IXAudio2SourceVoice** pSourceVoice, XAUDIO2_BUFFER** buffer)
		{
			if(*buffer){
				DXThrowIfFailed(pSourceVoice[0]->Start( 0 ) );
			}
			return true;
		}

		bool SoundSystem::stopSound(IXAudio2SourceVoice** pSourceVoice){
			pSourceVoice[0]->Stop();
			return true;
		}

		bool SoundSystem::menuVolume(){
			float volume;
			m_pMusicSub->GetVolume(&volume);
			m_pMusicSub->SetVolume(volume/8);
			m_pFXSub->GetVolume(&volume);
			m_pFXSub->SetVolume(volume/8);
			return true;
		}

		bool SoundSystem::resumeVolume(){
			float volume;
			m_pMusicSub->GetVolume(&volume);
			m_pMusicSub->SetVolume(volume*8);
			m_pFXSub->GetVolume(&volume);
			m_pFXSub->SetVolume(volume*8);
			return true;
		}

		bool SoundSystem::setMusicVolume(float volume){
			m_pMusicSub->SetVolume(volume);
			return true;
		}

		bool SoundSystem::setFXVolume(float volume){
			m_pFXSub->SetVolume(volume);
			return true;
		}

		float SoundSystem::getMusicVolume(){
			float volume;
			m_pMusicSub->GetVolume(&volume);
			return volume;
		}

		float SoundSystem::getFXVolume(){
			float volume;
			m_pFXSub->GetVolume(&volume);
			return volume;
		}

		bool SoundSystem::setMenuVolume(float volume){
			m_pMenuSub->SetVolume(volume);
			return true;
		}

		float SoundSystem::getMenuVolume(){
			float volume;
			m_pMenuSub->GetVolume(&volume);
			return volume;
		}
	
		HRESULT SoundSystem::findChunkData(HANDLE hFile, DWORD fourcc, DWORD & dwChunkSize, DWORD & dwChunkDataPosition)
		{
			HRESULT hr = S_OK;
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, 0, NULL, FILE_BEGIN ) )
				return HRESULT_FROM_WIN32( GetLastError() );

			DWORD dwChunkType;
			DWORD dwChunkDataSize;
			DWORD dwRIFFDataSize = 0;
			DWORD dwFileType;
			DWORD bytesRead = 0;
			DWORD dwOffset = 0;

			while (hr == S_OK)
			{
				DWORD dwRead;
				if( 0 == ReadFile( hFile, &dwChunkType, sizeof(DWORD), &dwRead, NULL ) )
					hr = HRESULT_FROM_WIN32( GetLastError() );

				if( 0 == ReadFile( hFile, &dwChunkDataSize, sizeof(DWORD), &dwRead, NULL ) )
					hr = HRESULT_FROM_WIN32( GetLastError() );

				switch (dwChunkType)
				{
				case fourccRIFF:
					dwRIFFDataSize = dwChunkDataSize;
					dwChunkDataSize = 4;
					if( 0 == ReadFile( hFile, &dwFileType, sizeof(DWORD), &dwRead, NULL ) )
						hr = HRESULT_FROM_WIN32( GetLastError() );
					break;

				default:
					if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, dwChunkDataSize, NULL, FILE_CURRENT ) )
					return HRESULT_FROM_WIN32( GetLastError() );            
				}

				dwOffset += sizeof(DWORD) * 2;
        
				if (dwChunkType == fourcc)
				{
					dwChunkSize = dwChunkDataSize;
					dwChunkDataPosition = dwOffset;
					return S_OK;
				}

				dwOffset += dwChunkDataSize;
        
				if (bytesRead >= dwRIFFDataSize) return S_FALSE;

			}

			return S_OK;
		}

		HRESULT SoundSystem::readChunkData(HANDLE hFile, void * buffer, DWORD buffersize, DWORD bufferoffset)
		{
			HRESULT hr = S_OK;
			if( INVALID_SET_FILE_POINTER == SetFilePointer( hFile, bufferoffset, NULL, FILE_BEGIN ) )
				return HRESULT_FROM_WIN32( GetLastError() );
			DWORD dwRead;
			if( 0 == ReadFile( hFile, buffer, buffersize, &dwRead, NULL ) )
				hr = HRESULT_FROM_WIN32( GetLastError() );
			return hr;
		}

		void SoundSystem::initializeBuffers(const int& levelNo){

			while(!buffers.empty()){
				buffers.pop_back();
			}

			Serializer Seri;

			std::stringstream ss;

			std::string filepath = "Assets/Audio/XML/";

			ss<<filepath;
			ss<<levelNo;

			std::string filename = ss.str();

			ThrowErrorIf(!Seri.LoadFile(filename), "Error with Audio XML file. Try checking closing tags or make sure the level file exists.");
			
			Seri.LoadFirstItem();
			std::string name;
			std::string audioFile;
			std::string type;
				
			do {
				std::stringstream ss2;
				name = Seri.GetElementName();
				type = Seri.GetAttributeString("type");
				audioFile = Seri.GetAttributeString("name");

				filepath = "Assets\\Audio\\Wav\\";
				ss2<<filepath;
				ss2<<audioFile;
				ss2<<".wav";

				filename = ss2.str();
				buffers.push_back(bufferSpace(const_cast<char *>(filename.c_str()), name, type));
			} while(Seri.NextNode());

			setMusic(findBuffer("Music"));
			setCollision(findBuffer("Collision"));
			setMenuClick(findBuffer("MenuClick"));

		}

		XAUDIO2_BUFFER* SoundSystem::findBuffer(std::string name){
			if(name == "0")
				return NULL;
			for(unsigned int i = 0; i < buffers.size(); i++){
				if(name == *buffers[i].getName()){
					return buffers[i].getBuffer();
				}
			}

			return NULL;
		}

		void SoundSystem::clearBuffer(){
			while(!buffers.empty()){
				bufferSpace temp = buffers.back();
				temp.deleteBuffer();
				buffers.pop_back();
			}
		}

		void SoundSystem::stopAllSound(){
			stopMusic();
			stopCollision();
			ObjectLinkList<Audio>::iterator it = m_AudioObjects.begin();
			for(;it!=m_AudioObjects.end();++it)
			{
				it->stop();
			}	
		}

		void SoundSystem::RemoveAudioComponent(int id){
			ObjectLinkList<Audio>::iterator it = m_AudioObjects.begin();
			for(;it!=m_AudioObjects.end();++it)
			{
				if((*it).GetOwnerID() == id)
				{
					it->stop();
					m_AudioObjects.erase(it);
					return;	
				}
			}
		}

}