/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GamePlay.h
Purpose: Header for GamePlay.cpp
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: CS529_pcristina_Final
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#pragma once

#include "ISystem.h"
#include "Message.h"
#include "Factory.h"
#include "SoundSystem.h"

#include "GameObject.h"

#include "Spawner.h"
#include "Trigger.h"
#include "Particle.h"

#include "GameStateManager.h"

#include "Mechanics.h"


#define EPSILON 0.0001
#define STARTING_LEVEL 1
#define MAX_LEVEL 2

namespace KrakEngine{
	enum AnimationState;
	class Controller;
	class CollisionHandler;
	class LevelEditor;
	class MenuManager;
	class Timer;

	class Gameplay : public ISystem{

	public:

		friend class ControllerSystem;

		Gameplay();
		~Gameplay();

		void HandleMessages(Message* message);
		void Update(float dt);
		bool Initialize();
		bool Shutdown();

		bool InitializeGame();

		//timer list 
		std::list<Timer*> TimerList;
		std::list<Spawner*> SpawnerList;
		std::list<Trigger*> TriggerList;
		std::list<Particle*> ParticleList;
		
		inline Mechanics & GetMechanics() { return m_gameMechanics; };

		void EmplaceNewObjectOnLevelEditorList(GameObject * go);
		void EmplaceNewObjectOnLevelEditorList(std::string name);

		void GamePlayLoadMainMenuLevel();

		MenuManager * m_MenuManager;

		float GetCurrentTrack();

		LevelEditor * CurrentLevelEditor;

		void SetLevelDepth(float depth) { m_LevelDepth = depth; }
		float GetLevelDepth() { return m_LevelDepth; }
		void ZoomOut();
		void ZoomIn();


		bool IsSpedUp() {return SpeedUpCheat;}
		void SetSpeedUpCheatTo(bool toggle) { SpeedUpCheat = toggle; }

		bool IsDoubleJump() {return CheatDoubleJump;}
		void SetDoubleJumpTo(bool toggle) { CheatDoubleJump = toggle; }
	private:

		void LoadLevelFile(const int& levelNo);

		void UnloadLevel();

		void InGame(float dt);
		void Restart();
		void Lost();
		void Win(float dt);
		void LevelEditorState();
		void InMenu();
		void StartGame();
		void LoadingInitial();
		void LoadingRestart();		
		void Digipen(float dt);
		void GoNinja(float dt);
		void ControllerRequired(float dt);
		void DisplayVictory(float dt);
		void PauseGame();

		void SceneOne(float dt);
		void SceneTwo(float dt);
		void SceneThree(float dt);
		void SceneFour(float dt);

		void SetUpDisplayState(std::string imagename);
		void ExitTo(GameStates::GameState StateToGoTo);

		void Tick();
		void Spawn(float dt);
		void Trigger();
		void ParticleRun();

		void AdjustZCamera();
		void AdjustYCamera();

		void MoveCursor();
		void HideCursor();

		void Gameplay::UpdateUI(bool gamepad);

		void Jump();

		void SetIsPlayerFacingRight(bool isFacingRight);
		void SetAnimationState(AnimationState as);

		//points to the object with control (usually hat)
		GameObject * Player;

		GameObject * Cursor;

		GameObject * Test;

		void ResetCursor();

		void TurnOffCheats();
		bool SpeedUpCheat;
		bool CheatDoubleJump;

		void RemoveTimerComponent(int objectid);
		void RemoveSpawnerComponent(int objectid);
		void RemoveTriggerComponent(int objectid);
		void RemoveParticleComponent(int objectid);

		CollisionHandler * ColHandler;
	
		Mechanics m_gameMechanics;

		float zTarget;
		bool isZoomDirectionIn;
		bool zCamerachange;
		float prevz;

		float zCurrent;
		float zstep;
		float totalsteps;

		float ystep;

		float m_LevelDepth;

		float m_Time;

		GameObject * m_backimage;

		float m_TimeInState;

		AnimationState m_AnimationState;

		bool m_musicFlag;

        XMFLOAT2 m_LastMouseState;
        XMFLOAT2 m_CurrentMouseState;
	};

	extern Gameplay* g_GAMEPLAYSYSTEM;
}
