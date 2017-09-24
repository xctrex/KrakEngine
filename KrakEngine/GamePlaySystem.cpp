/* Start Header -------------------------------------------------------
Copyright (C) 2014 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GamePlay.cpp
Purpose: Game Logic Programming
Language: C++, MSC
Platform: 10.0 , Windows 7
Project: Go Ninja Hat-Life
Author: Cristina Pohlenz, p.cristina
Creation date: 12/15/2013
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "GamePlaySystem.h"
#include "InputSystem.h"
#include "DrawingState.h"
#include "Transform.h"
#include "Sprite.h"
#include "RigidBody.h"
#include "Core.h"
#include "Model.h"
#include "Audio.h"
#include "Action.h"
#include "Controller.h"
#include "ControllerSystem.h"
#include "LuaInterpreter.h"
#include "AIComp.h"
#include "AISystem.h"
#include "Text.h"
#include "CollisionHandler.h"
#include "LevelEditor.h"
#include "MenuManager.h"
#include "Image.h"
#include "Camera.h"
#include "Timer.h"

#include <iostream>

#define LEVEL_EDITOR_LEVEL 200
#define MAINMENULEVEL 1
#define LOADING 999

#define TRACK_DEPTH 4.0f

namespace KrakEngine{

	Gameplay* g_GAMEPLAYSYSTEM = NULL;
	Gameplay::Gameplay() : Player(NULL), ColHandler(new CollisionHandler()), m_MenuManager(new MenuManager()), CurrentLevelEditor(NULL), zTarget(0.0f), zCamerachange(false), m_Time(0.0f), m_backimage(NULL), SpeedUpCheat(false), CheatDoubleJump(false) {
		g_GAMEPLAYSYSTEM = this;
		srand ((unsigned int)time(NULL));
	};

	Gameplay::~Gameplay()
	{
		delete ColHandler;
	}

	void Gameplay::HandleMessages(Message* message){

		if(message->MessageId == MId::ObjectDestroyed){
			ObjectDestroyedMessage * ODM = dynamic_cast<ObjectDestroyedMessage *>(message);
			RemoveTimerComponent(ODM->GetID());
			RemoveSpawnerComponent(ODM->GetID());
			RemoveTriggerComponent(ODM->GetID());
			RemoveParticleComponent(ODM->GetID());
		}
		else if (message->MessageId == MId::ClearComponentLists){
		/*	
			auto it = TimerList.begin();
			for( ; it != TimerList.end(); ++it)
				delete (*it);
			*/
			TimerList.clear();
			SpawnerList.clear();
			TriggerList.clear();
			ParticleList.clear();
		}
		else if (message->MessageId == MId::Collision)
		{
				CollisionMessage * CM = dynamic_cast<CollisionMessage *>(message);
				ColHandler->FindCollision(CM);
		}
		else if (message->MessageId == MId::GamePadActive)
		{
			GamePadActiveMessage * GAM = dynamic_cast<GamePadActiveMessage *>(message);
			if (!GAM->IsControllerActive() && CORE->GetCurrentState() == GameStates::InGame){
				UpdateUI(false);
				PauseGame();
			}
			else if (GAM->IsControllerActive())
			{
				UpdateUI(true);
			}
		}
		else if(message->MessageId == MId::Active){
				ActiveMessage * ODM = dynamic_cast<ActiveMessage *>(message);
			if(!ODM->GetActive() && CORE->GetCurrentState() == GameStates::InGame){
				PauseGame();
			}
		}
	}

	void Gameplay::PauseGame(){
#ifndef _DEBUG		
        if(!m_MenuManager->GetCurrentMenu())
		{
			m_MenuManager->CreateNewMenu(Pause);
			HideCursor();
			CORE->SetNextState(GameStates::InMenu);
		}
#endif
	}

	void Gameplay::UpdateUI(bool gamepad){
		std::list<GameObject*> guilist = g_FACTORY->FindByTypeList("UI");
		
		if(guilist.size() == 0)
			return;

		std::list<GameObject*>::iterator it = guilist.begin();
		for ( ; it != guilist.end(); ++it )
		{
			Sprite* sg = (*it)->has(Sprite);
			std::string texName = sg->GetTextureName();

			if (!gamepad){
				texName.erase(std::remove(texName.begin(), texName.end(), '2'), texName.end());
			}
			else{
				texName.append("2");
			}
			sg->SetTextureName(texName);
		}

	}


	void Gameplay::Update(float dt){
		GameStates::GameState currentState = CORE->GetCurrentState();

		switch(currentState)
		{
		case GameStates::InGame : 
			{
				InGame(dt);
				break;
			}

		case GameStates::Restart :
			{
				Restart();
				break;
			}
		case GameStates::GameOver :
			{
				Lost();
				break;
			}
		case GameStates::Victory :
			{
				Win(dt);
				break;
			}
		case GameStates::VictoryScene :
			{
				DisplayVictory(dt);
				break;
			}
		case GameStates::StartGame :
			{
				StartGame();
				break;
			}
		case GameStates::InMenu :
			{
				InMenu();
				break;
			}

		case GameStates::LevelEdit :
			{
				LevelEditorState();
				break;
			}
		case GameStates::LoadingInitial :
			{
				LoadingInitial();
				break;
			}
		case GameStates::LoadingRestart :
			{
				LoadingRestart();
				break;
			}
		case GameStates::Digipen :
			{
				//Digipen(dt);
                //Hack - go straight into loading the initial level
                StartGame();
				break;
			}
		case GameStates::GoNinja :
			{
				GoNinja(dt);
				break;
			}
		case GameStates::ControllerRequired :
			{
				ControllerRequired(dt);
				break;
			}
		case GameStates::SceneOne :
			{	
				SceneOne(dt);
				break;
			}
		case GameStates::SceneTwo :
			{	SceneTwo(dt);
				break;
			}
		case GameStates::SceneThree :
			{	SceneThree(dt);
				break;
			}
		case GameStates::SceneFour :
			{	SceneFour(dt);
				break;
			}

		}
	}

	/// <summary>
	/// Ins the game.
	/// </summary>
    void Gameplay::InGame(float dt)
    {
        g_INPUTSYSTEM->UnpauseVibration();
        GameObject * currentcontrol = g_CONTROLLERSYSTEM->FindController();

        if (currentcontrol)
            if (currentcontrol->GetType() != "mime")
                HideCursor();
            else
            {
                Image * ic = Cursor->has(Image);
                XMFLOAT3 cursor_pos = ic->GetPosition();


            }

        CurrentLevelEditor = NULL;
        //rundown timers

        m_Time += dt;

        if (m_Time > 0.03f)
        {
            Tick();
            Spawn(dt);
            Trigger();
           /* AdjustZCamera();
            AdjustYCamera();*/ 
            ParticleRun();
            m_gameMechanics.Update();
            m_Time = 0.0f;
        }

        if (Player)
        {
            Controller * cc = Player->has(Controller);
            RigidBody* playerPhysics = NULL;

            if (cc->IsCurrentController())
            {
                playerPhysics = Player->has(RigidBody);
            }

            Transform *playerTransform = Player->has(Transform);

            // If the player is on the ground, set animation to idle by default (will be overridden later if the player is running)
            if (playerPhysics->collision & COLLISION_BOTTOM) {
                SetAnimationState(AnimationStateIdle);
            }

            /****MOVE DOWN****/
            if (g_INPUTSYSTEM->IsKeyTriggered(DIK_DOWN) ||
                g_INPUTSYSTEM->IsKeyTriggered(DIK_S) ||
                g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_DPAD_DOWN))
            {
                /*Controller * c = g_CONTROLLERSYSTEM->FindController()->has(Controller);
                if(c->IsImmobile()) return;

                XMFLOAT3 newPos = playerTransform->GetPosition();

                if(newPos.z != -TRACK_DEPTH)
                {
                    prevz = newPos.z;

                    newPos.z -= TRACK_DEPTH;

                    if(newPos.z == 0)
                        zTarget = m_LevelDepth;
                    else
                        zTarget = m_LevelDepth - TRACK_DEPTH;

                    zCurrent = zTarget + TRACK_DEPTH;

                    isZoomDirectionIn = false;
                    zCamerachange = true;

                    zstep = 0;
                }

                playerTransform->SetPosition(newPos);*/

            }
            /****MOVE UP****/
            if (g_INPUTSYSTEM->IsKeyTriggered(DIK_UP) ||
                g_INPUTSYSTEM->IsKeyTriggered(DIK_W) ||
                g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_DPAD_UP))
            {
                Controller * c = g_CONTROLLERSYSTEM->FindController()->has(Controller);
                if (c->IsImmobile()) return;

                XMFLOAT3 newPos = playerTransform->GetPosition();

                if (newPos.z != TRACK_DEPTH)
                {
                    prevz = newPos.z;

                    newPos.z += TRACK_DEPTH;
                    if (newPos.z == 0)
                        zTarget = m_LevelDepth;
                    else
                        zTarget = m_LevelDepth + TRACK_DEPTH;

                    ////Get Previous Depth
                    //Camera * cam = g_GRAPHICSSYSTEM->GetCurrentCamera();
                    //float camDepth = cam->GetDepth();

                    ////Add 4 to it
                    //zTarget = cam->GetDepth() + TRACK_DEPTH;

                    zCurrent = zTarget - TRACK_DEPTH;

                    isZoomDirectionIn = true;
                    zCamerachange = true;

                    zstep = 0;
                }

                playerTransform->SetPosition(newPos);

            }
            /****MOVE LEFT****/
            if (g_INPUTSYSTEM->IsKeyDown(DIK_LEFT) ||
                g_INPUTSYSTEM->IsKeyDown(DIK_A) ||
                g_INPUTSYSTEM->IsButtonDown(XINPUT_GAMEPAD_DPAD_LEFT))
            {
                Controller * c = g_CONTROLLERSYSTEM->FindController()->has(Controller);
                if (c->IsImmobile()) return;

                SetIsPlayerFacingRight(false);

                // If the player is on the ground, set animation to run
                if (playerPhysics->collision & COLLISION_BOTTOM) {
                    SetAnimationState(AnimationStateRunning);
                }

                if (playerPhysics) {
                    playerPhysics->velocity.x = -(playerPhysics->speed);
                }

            }
            /****MOVE RIGHT****/
            else if (g_INPUTSYSTEM->IsKeyDown(DIK_RIGHT) ||
                g_INPUTSYSTEM->IsKeyDown(DIK_D) ||
                g_INPUTSYSTEM->IsButtonDown(XINPUT_GAMEPAD_DPAD_RIGHT))
            {
                Controller * c = g_CONTROLLERSYSTEM->FindController()->has(Controller);
                if (c->IsImmobile()) return;

                SetIsPlayerFacingRight(true);

                // If the player is on the ground, set animation to run
                if (playerPhysics->collision & COLLISION_BOTTOM) {
                    SetAnimationState(AnimationStateRunning);
                }

                if (playerPhysics) {
                    playerPhysics->velocity.x = playerPhysics->speed;
                }
            }
            else {
                if (playerPhysics)
                    playerPhysics->velocity.x = 0.0f;
            }

            /****MOVE PLAYER WITH LEFT THUMB STICK****/


            /*
            if(g_INPUTSYSTEM->IsLeftThumbStickTriggered(ThumbX, ThumbY))
            {
                playerPhysics->velocity.x += (ThumbX / 5000.0f);

                if(LeftThumbStickTimer)
                    return;

                if(ThumbY > 25000)
                {
                    XMFLOAT3 newPos = playerTransform->GetPosition();
                    if(newPos.z != TRACK_DEPTH)
                        newPos.z += TRACK_DEPTH;
                    playerTransform->SetPosition(newPos);
            }
                else if (ThumbY < -25000)
                {
                    XMFLOAT3 newPos = playerTransform->GetPosition();
                if(newPos.z != -TRACK_DEPTH)
                    newPos.z -= TRACK_DEPTH;
                    playerTransform->SetPosition(newPos);
                }

                LeftThumbStickTimer = new CounterTimer(20, "LeftThumbStickTimer");

            }
            */
            //****JUMP****//
            if (g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) ||
                g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A))
            { //is space being pressed
                Jump();
            }

            /****RESET CURSOR TO PLAYER POSITION****/
            if (g_INPUTSYSTEM->IsButtonDown(XINPUT_GAMEPAD_LEFT_THUMB) || g_INPUTSYSTEM->IsMouseButtonTriggered(MouseButton::Middle))
            {
                if (Cursor)
                {
                    XMFLOAT4X4 identity;
                    XMStoreFloat4x4(&identity, XMMatrixIdentity());
                    Image * tc = Cursor->has(Image);
                    Transform *tc2 = Player->has(Transform);
                    XMFLOAT3 pos = tc2->GetPosition();
                    XMStoreFloat4x4(&identity, XMMatrixIdentity());
                    XMFLOAT2 screencoord = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(pos, identity);
                    XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();
                    pos.x = screencoord.x / screensize.x;
                    pos.y = screencoord.y / screensize.y;
                    pos.z = 100.0f;
                    tc->SetPosition(pos);
                }
            }


            if (g_CONTROLLERSYSTEM->FindController()->GetType() == "mime")
                /****MOVE CURSOR WITH MOUSE****/
                if (g_INPUTSYSTEM->HasMouseMoved()) {
                    MoveCursor();
                }

            /****MOVE CURSOR WITH RIGHT THUMB STICK****/
            float ThumbX, ThumbY;
            if (g_INPUTSYSTEM->IsRightThumbStickTriggered(ThumbX, ThumbY))
            {
                if (Cursor)
                {
                    Image * tc = Cursor->has(Image);
                    if (tc)
                    {
                        XMFLOAT3 pos = tc->GetPosition();
                        pos.x += (ThumbX / 8000000.0f);
                        pos.y -= (ThumbY / 8000000.0f);

                        if (pos.x > 1.0f)
                            pos.x = 1.0f;
                        else if (pos.x < 0.0f)
                            pos.x = 0.0f;

                        if (pos.y > 1.0f)
                            pos.y = 1.0f;
                        else if (pos.y < 0.0f)
                            pos.y = 0.0f;


                        tc->SetPosition(pos);
                    }

                }
            }
        }

        /****CAMERA MOVEMENT****/
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_E))
        {
            g_GRAPHICSSYSTEM->ToggleEditPath();
        }
        /*Camera * cam = g_GRAPHICSSYSTEM->GetCurrentCamera();
        if (cam && !g_GRAPHICSSYSTEM->IsEditPathOn()) {
            float LookSpeed = 40.0f * dt;
            float speed = 1000.0f * dt;
            if (g_INPUTSYSTEM->IsKeyDown(DIK_A))
            {
                cam->MoveLeftRight(-2.0f * speed);
            }
            else if (g_INPUTSYSTEM->IsKeyDown(DIK_D))
            {
                cam->MoveLeftRight(2.0f * speed);
            }
            else
            {
                cam->MoveLeftRight(0.0f);
            }

            if (g_INPUTSYSTEM->IsKeyDown(DIK_W))
            {
                cam->MoveBackForward(speed);
            }
            else if (g_INPUTSYSTEM->IsKeyDown(DIK_S))
            {
                cam->MoveBackForward(-speed);
            }
            else
            {
                cam->MoveBackForward(0.0f);
            }

            if (g_INPUTSYSTEM->IsKeyDown(DIK_Z))
            {
                cam->MoveUpDown(-speed);
            }
            else if (g_INPUTSYSTEM->IsKeyDown(DIK_X))
            {
                cam->MoveUpDown(speed);
            }
            else
            {
                cam->MoveUpDown(0.0f);
            }

            if (g_INPUTSYSTEM->HasMouseMoved())
            {
                cam->AdjustYaw(g_INPUTSYSTEM->getMouseX() * 0.0005f / LookSpeed);

                cam->AdjustPitch(g_INPUTSYSTEM->getMouseY() * 0.0005f / LookSpeed);
            }

            //cam->UpdateCamera();
        }*/

        /****ACTION 1 BUTTON****/
        if (g_INPUTSYSTEM->IsButtonDown(XINPUT_GAMEPAD_B) ||
            g_INPUTSYSTEM->IsLeftTriggerJustPressed() ||
            g_INPUTSYSTEM->IsMouseButtonDown(MouseButton::Left))
        {
            if (Player)
            {
                /*
                Audio * a = Player->has(Audio);
                    if(a)
                        a->playAction1();
                */
                Action *ac = Player->has(Action);
                if (ac)
                {
                    ac->PerformAction1(Player);
                }
            }
        }
        else {
            if (Player)
            {
                Particle* p = Player->has(Particle);
                if (p)
                    p->setScript("");

            }
        }

        ///****ACTION 2 BUTTON****/
        //if(	g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_Y) ||
        //	g_INPUTSYSTEM->IsRightTriggerJustPressed() ||
        //	g_INPUTSYSTEM->IsMouseButtonTriggered(MouseButton::Right))
        //{
        //	if(Player)
        //	{
        //		Audio * a = Player->has(Audio);
        //			if(a)
        //				a->playAction2();

        //		Action *ac = Player->has(Action);
        //		if(ac)
        //		{
        //			ac->PerformAction2(Player);
        //		}
        //	}
        //}

        /****REMOVE HAT****/
        /*if(g_INPUTSYSTEM->IsKeyTriggered(DIK_E) ||
            g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_X))
        {
            Particle* p = Player->has(Particle);
            if(p)
                p->setScript("");


            Sprite * sc = Player->has(Sprite);
            if(sc)
            {
                sc->SetAnimationState(AnimationStateIdle);
            }

            g_CONTROLLERSYSTEM->ReturnControlToHat();




        }*/

        /****PAUSE****/
        //Cristina 20140324: Pause Menu should be called by ESC or START according to TCRs
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE) ||
            g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START))
        {
            m_MenuManager->CreateNewMenu(Pause);
            CORE->SetNextState(GameStates::InMenu);
        }

        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_F12))
        {
            m_MenuManager->CreateNewMenu(CheatMenu);
            CORE->SetNextState(GameStates::InMenu);
        }

        if (Player)
        {
            RigidBody *rb = Player->has(RigidBody);
            if (!SpeedUpCheat)
                rb->speed = 6.0f;
            else
                rb->speed = 20.0f;
        }

        //All controlls defined here are for debug uses only and should not be included in release
        /****DEBUG****/
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_Q))
        {
            CORE->SetNextState(GameStates::Quit);
        }
		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_G) ||
			g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{            
            g_DRAWSTATE->CycleDrawingMode();
		}
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_T))
        {
            g_DRAWSTATE->CycleTechnique();
        }
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_M))
        {            
            g_GRAPHICSSYSTEM->ToggleModel();
        }
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_L))
        {
            g_DRAWSTATE->m_isLightDynamic = !g_DRAWSTATE->m_isLightDynamic;
        }
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_R))
        {
            g_DRAWSTATE->m_isStrokeDirectionDynamic = !g_DRAWSTATE->m_isStrokeDirectionDynamic;
        }
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_B))
        {
            g_DRAWSTATE->m_isGradientBufferBlurOn = !g_DRAWSTATE->m_isGradientBufferBlurOn;
        }
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_C))
        {
            g_DRAWSTATE->m_numBlurPasses++;
        }
        if (g_INPUTSYSTEM->IsKeyTriggered(DIK_X))
        {
            g_DRAWSTATE->m_numBlurPasses--;
        }

#if defined(_DEBUG)
        // TODO: Remove this. It is only being used to test the ConvertToWorldCoordinates function
        if(g_INPUTSYSTEM->IsKeyTriggered(DIK_V) ||
			g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_RIGHT_SHOULDER))
		{          
            XMFLOAT3 wcLeft = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(XMFLOAT2(0.25f, 0.5f));
            XMFLOAT3 wcRight = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(XMFLOAT2(0.75f, 0.5f));
            XMFLOAT3 wcTop = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(XMFLOAT2(0.5f, 0.25f));
            XMFLOAT3 wcBottom = g_GRAPHICSSYSTEM->ConvertToWorldCoordinates(XMFLOAT2(0.5f, 0.75f));
		}

		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_8))
		{
			Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();
			c->IsFreeCameraSet() ? c->SetFreeCamera(false) : c->SetFreeCamera(true);
		}
		/****RESTART****/
		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_R))
		{
			CORE->SetNextState(GameStates::Restart);
		}

		/****Cristina 20140324: Controls to change levels // Debugging*****/
		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_9))
		{
			CORE->SetNextLevel(98);
		}
		
		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_0) || 
			g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_LEFT_SHOULDER))
		{
			if(CORE->GetCurrentLevel() + 1 > MAX_LEVEL)
				CORE->SetNextState(GameStates::VictoryScene);
			else
				CORE->SetNextLevel(CORE->GetCurrentLevel() + 1);
		}

		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_6))
		{
  			Controller * cc = Player->has(Controller);
			if(cc->IsCurrentController())
				cc->SetPlayerStatus(false);
			else
				cc->SetPlayerStatus(true);
		}

		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_F)){
			Message m(MId::ToggleFullScreen);
            CORE->Broadcast(&m);
        }

		//DEBUG
		if(g_INPUTSYSTEM->IsKeyTriggered(DIK_H))
		{
			LuaInterpreter Li;
			
			Li.SetCurrentObj(Player);
			Li.LoadScript("resetcamera");
		}
#endif

		
		if(CORE->GetCurrentLevel() != CORE->GetNextLevel())
		{
			UnloadLevel();
			int current = CORE->GetNextLevel();
			CORE->SetNextLevel(LOADING);
			CORE->ChangeLevel();
			LoadLevelFile(CORE->GetCurrentLevel());
			CORE->SetNextState(GameStates::LoadingRestart);
			CORE->SetNextLevel(current);
			CORE->ChangeLevel();
		}
	}
		
	void Gameplay::InMenu()
	{
		g_PHYSICSSYSTEM->PausePhysics();
		g_AISYSTEM->PauseAI();
		g_INPUTSYSTEM->PauseVibration();



		//default menu is the main menu
		if(!m_MenuManager->GetCurrentMenu()) 
			m_MenuManager->CreateNewMenu(MenuType::Start);	

		if (m_MenuManager->GetCurrentMenuType() == MenuType::Start && !m_musicFlag)
		{
			g_SOUNDSYSTEM->playMusic();
			m_musicFlag=true;
		}else if (m_MenuManager->GetCurrentMenuType() != MenuType::Start){
			m_musicFlag=false;
		}
		

		if(!g_INPUTSYSTEM->IsMouseButtonTriggered(Left))
			m_MenuManager->LeftMouseRelease();

		m_MenuManager->CheckMousePosition();
		m_MenuManager->DrawOptions();
				
#if defined(_DEBUG)
			
		if( g_INPUTSYSTEM->IsKeyTriggered(DIK_Q)){
				CORE->SetNextState(GameStates::Quit);
			}
#endif

		if (g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE) ||
			g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START))
		{
            CORE->SetNextState(GameStates::Quit);
			/*if (m_MenuManager->GetCurrentMenuType() == MenuType::Pause){
				m_MenuManager->DeleteCurrentMenu();
				CORE->SetNextState(GameStates::InGame);
			}*/
		}


		if (g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A) ||
				g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN)
			)
		{
			m_MenuManager->SelectOption();
		}

		if (g_INPUTSYSTEM->IsKeyTriggered(DIK_UP) ||
				g_INPUTSYSTEM->IsKeyTriggered(DIK_W) ||
				g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_DPAD_UP))
			{ 
				m_MenuManager->Up();
			}

		if (g_INPUTSYSTEM->IsKeyTriggered(DIK_DOWN) ||
				g_INPUTSYSTEM->IsKeyTriggered(DIK_S) ||
				g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_DPAD_DOWN)) 
		{
			m_MenuManager->Down();
		}
		
		if (g_INPUTSYSTEM->IsKeyTriggered(DIK_LEFT) ||
				g_INPUTSYSTEM->IsKeyTriggered(DIK_A) ||
				g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_DPAD_LEFT))
			{ 
				m_MenuManager->Left();
			}

		if (g_INPUTSYSTEM->IsKeyTriggered(DIK_RIGHT) ||
				g_INPUTSYSTEM->IsKeyTriggered(DIK_D) ||
				g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_DPAD_RIGHT)) 
		{
			m_MenuManager->Right();
		}

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left))
		{
			m_MenuManager->MouseClick();
		}
	
		if(g_INPUTSYSTEM->HasMouseMoved()){
			MoveCursor();
		}
	}

	void Gameplay::Lost(){
		g_FACTORY->DeleteAllObjects();
	}

	void Gameplay::Win(float dt){

		//SetUpDisplayState("victory");

		m_TimeInState += dt;
		m_Time += dt;
		GameObject * ladyHat = g_FACTORY->FindByType("femalehat");
		Audio* a = ladyHat->has(Audio);
		if (a)
			a->playAction1();
		

		if(m_Time > 0.03f)
		{
			Tick();
			ParticleRun();
			m_Time = 0.0f;
		}

		if( m_TimeInState > 4.0f )
		{
			g_SOUNDSYSTEM->stopAllSound();
			g_SOUNDSYSTEM->clearBuffer();
			g_SOUNDSYSTEM->initializeBuffers(3);
			ExitTo(GameStates::VictoryScene);
		}
	}

	void Gameplay::StartGame(){
		UnloadLevel();
		CORE->SetNextLevel(LOADING);
		CORE->ChangeLevel();
		LoadLevelFile(CORE->GetCurrentLevel());
		CORE->SetNextState(GameStates::LoadingInitial);
	}

	void Gameplay::ExitTo(GameStates::GameState StateToGoTo)
	{
		if(m_backimage)
			m_backimage->Kill();
		m_backimage = NULL;
		CORE->SetNextState(StateToGoTo);
	}
	
	void Gameplay::SetUpDisplayState(std::string imagename)
	{
		if(m_backimage)
			return;
		
		m_backimage = g_FACTORY->Create("ImageObject");
		Image * ic = m_backimage->has(Image);

		ic->SetImageName(imagename);
		
		XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();

		ic->SetSize((float)screensize.x, (float)screensize.y);

		ic->SetPosition(XMFLOAT3(0.5f, 0.5f, 0.0f));
	
		m_TimeInState = 0.0f;
	}

	void Gameplay::Digipen(float dt)
	{
		SetUpDisplayState("digipenlogo");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			)
		{
			//ExitTo(GameStates::GoNinja);
			ExitTo(GameStates::InMenu);
		}else if( m_TimeInState > 3.0f){
			ExitTo(GameStates::GoNinja);
		}
		
		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		*/
	}

	void Gameplay::GoNinja(float dt)
	{
		SetUpDisplayState("goninjalogo");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			)
		{
			//ExitTo(GameStates::ControllerRequired);
			ExitTo(GameStates::InMenu);
		}
		else if( m_TimeInState > 3.0f){
			ExitTo(GameStates::ControllerRequired);
		}

		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		*/
	}
	
	void Gameplay::ControllerRequired(float dt)
	{
		SetUpDisplayState("howtoplay");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			|| m_TimeInState > 3.0f
			)
		{
			ExitTo(GameStates::InMenu);
		}
		
		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		`*/
	}

	void Gameplay::DisplayVictory(float dt)
	{
		g_SOUNDSYSTEM->playMusic();
		SetUpDisplayState("victory");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			|| m_TimeInState > 10.0f
			)
		{
			ExitTo(GameStates::InMenu);
		}
	}

	void Gameplay::SceneOne(float dt)
	{
		m_musicFlag = false;
		g_SOUNDSYSTEM->playMusic();
		SetUpDisplayState("sceneOne");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			)
		{
			//ExitTo(GameStates::ControllerRequired);
			ExitTo(GameStates::SceneTwo);
		}
		else if( m_TimeInState > 3.0f){
			ExitTo(GameStates::SceneTwo);
		}

		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		*/
	}

	void Gameplay::SceneTwo(float dt)
	{
		SetUpDisplayState("sceneTwo");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			)
		{
			//ExitTo(GameStates::ControllerRequired);
			ExitTo(GameStates::SceneThree);
		}
		else if( m_TimeInState > 3.0f){
			ExitTo(GameStates::SceneThree);
		}

		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		*/
	}

	
	void Gameplay::SceneThree(float dt)
	{
		SetUpDisplayState("sceneThree");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			)
		{
			//ExitTo(GameStates::ControllerRequired);
			ExitTo(GameStates::SceneFour);
		}
		else if( m_TimeInState > 3.0f){
			ExitTo(GameStates::SceneFour);
		}

		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		*/
	}

	void Gameplay::SceneFour(float dt)
	{
		SetUpDisplayState("sceneFour");

		m_TimeInState += dt;

		if(g_INPUTSYSTEM->IsMouseButtonTriggered(Left) || g_INPUTSYSTEM->IsMouseButtonTriggered(Right)
			|| g_INPUTSYSTEM->IsKeyTriggered(DIK_RETURN) || g_INPUTSYSTEM->IsKeyTriggered(DIK_SPACE) || g_INPUTSYSTEM->IsKeyTriggered(DIK_ESCAPE)
			|| g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_START) || g_INPUTSYSTEM->IsButtonTriggered(XINPUT_GAMEPAD_A)
			)
		{
			//ExitTo(GameStates::ControllerRequired);
			ExitTo(GameStates::StartGame);
		}
		else if( m_TimeInState > 3.0f){
			ExitTo(GameStates::StartGame);
		}

		//CURSOR MUST BE HIDDEN - Cristina 20143003
		HideCursor();
		/*
			if(g_INPUTSYSTEM->HasMouseMoved()){
				MoveCursor();
			}
		*/
	}

	void Gameplay::LevelEditorState()
	{
		HideCursor();

		if(!CurrentLevelEditor)
		{
			g_PHYSICSSYSTEM->PausePhysics();
			g_AISYSTEM->PauseAI();	

			int level = CORE->GetCurrentLevel();

			UnloadLevel();
			LoadLevelFile(LEVEL_EDITOR_LEVEL);

			CurrentLevelEditor = new LevelEditor(level);
			CurrentLevelEditor->LoadObjectTypesScript();
		
			g_GRAPHICSSYSTEM->DrawTriggers();	
		}
		CurrentLevelEditor->Update();
	}

	bool Gameplay::Initialize(){
		m_musicFlag = false;
		GamePlayLoadMainMenuLevel();
		return true;
	}

	bool Gameplay::InitializeGame(){ 
		CORE->SetNextLevel(STARTING_LEVEL);
		CORE->ChangeLevel();

		LoadLevelFile(CORE->GetCurrentLevel());
		


		//Player = g_FACTORY->GetObjList().FindByName("Player");
		Test = g_FACTORY->GetObjList().FindByName("Square");

		//find controller
		Player = g_CONTROLLERSYSTEM->FindController();

		Cursor = g_FACTORY->GetObjList().FindByName("Cursor");
		
		//Camera * cam = g_GRAPHICSSYSTEM->GetCurrentCamera();
		//m_LevelDepth = cam->GetDepth();

		UpdateUI(g_INPUTSYSTEM->IsGamePadActive());

		return true;
	}
	
	void Gameplay::LoadLevelFile(const int& levelNo){

		CORE->SetNextLevel(levelNo);
		CORE->ChangeLevel();

		g_SOUNDSYSTEM->stopAllSound();
		g_SOUNDSYSTEM->clearBuffer();
		g_SOUNDSYSTEM->initializeBuffers(CORE->GetCurrentLevel());
		if(CORE->GetCurrentLevel() <= 2)
			g_SOUNDSYSTEM->playMusic();
		Serializer Seri;

		std::stringstream ss;

		std::string filepath = "Assets/Levels/";

		ss<<filepath;
		ss<<levelNo;

		std::string filename = ss.str();

		ThrowErrorIf(!Seri.LoadFile(filename), "Error with XML file. Try checking closing tags or make sure the level file exists.");

		std::string script = Seri.ReadLevel();

		if(script != "")
		{
			LuaInterpreter li;
		
			ThrowErrorIf(!li.LoadScript(script), "Error with lua script. It may not be loaded or may be trying to run an invalid operation.");
		}
	
		g_CONTROLLERSYSTEM->ResetHat();

		Player = g_FACTORY->GetObjList().FindByName("Player");
		if(!Player)
			Player = g_FACTORY->GetObjList().FindByName("player");

		Cursor = g_FACTORY->GetObjList().FindByName("Cursor");

		if(Player)
		{
			Transform * tc = Player->has(Transform);

		Camera * cam = g_GRAPHICSSYSTEM->GetCurrentCamera();
		if(cam)
			m_LevelDepth = cam->GetDepth() - tc->GetPosition().z;

		ThrowErrorIf(m_LevelDepth > 0, "Level depth was set to greater than 0. That seems a little to far toward the level. Check camera depth and player position.");
		}

		m_gameMechanics.SetTriggeredByButton(NULL);
		
		std::list<KrakEngine::Trigger*>::iterator it = TriggerList.begin();
		for(;it!=TriggerList.end();++it)
		{
			if((*it)->GetTriggeredOnButtonPress() > 0)
			{
				m_gameMechanics.SetTriggeredByButton( (*it)->GetOwner() );
				return;
			}
		}	
			
	}

	void Gameplay::TurnOffCheats()
	{
		CheatDoubleJump = false;
		SpeedUpCheat = false;
	}

	void Gameplay::UnloadLevel()
	{
		TurnOffCheats();

		g_FACTORY->DeleteAllObjects();
		g_SOUNDSYSTEM->stopAllSound();
		g_SOUNDSYSTEM->clearBuffer();
	}

	bool Gameplay::Shutdown(){ 
		return true; 
	}

	void Gameplay::GamePlayLoadMainMenuLevel()
	{
		//Clean up just in case we arrived from an ingame state
		UnloadLevel();
		
		//load the main menu file which has a cursor with a camera.
		CORE->SetNextLevel(MAINMENULEVEL);
		CORE->ChangeLevel();

		LoadLevelFile(CORE->GetCurrentLevel());
	}

	void Gameplay::Restart()
	{
		UnloadLevel();
		int current = CORE->GetCurrentLevel();
		CORE->SetNextLevel(LOADING);
		CORE->ChangeLevel();
		LoadLevelFile(CORE->GetCurrentLevel());
		CORE->SetNextState(GameStates::LoadingRestart);
		CORE->SetNextLevel(current);
		CORE->ChangeLevel();
	}

	void Gameplay::MoveCursor()
	{
			if(Cursor)
				{
					Image * tc = Cursor->has(Image);
					//Transform * tc = Cursor->has(Transform);
					if(tc)
					{						
						XMFLOAT3 pos = tc->GetPosition();

						float x = g_INPUTSYSTEM->getMouseX();
						float y = g_INPUTSYSTEM->getMouseY();

						pos.x += (x/MOUSE_SENSITIVITY);
						pos.y += (y/MOUSE_SENSITIVITY);

						if(pos.x > 1.0f)
							pos.x = 1.0f;
						else if(pos.x < 0.0f)
							pos.x = 0.0f;

						if(pos.y > 1.0f)
							pos.y = 1.0f;
						else if(pos.y < 0.0f)
							pos.y = 0.0f;
						
						tc->SetPosition(pos);		
					}
				}
	}

	void Gameplay::HideCursor()
	{
			if(Cursor)
				{
					Image * tc = Cursor->has(Image);
					//Transform * tc = Cursor->has(Transform);
					if(tc)
					{						
						XMFLOAT3 pos = tc->GetPosition();
						pos.x = -5.1f;
						pos.y = -5.1f;

						tc->SetPosition(pos);		
					}
				}
	}


	//Tick gathers all timers and ticks them down
	void Gameplay::Tick()
	{

		std::list<Timer*>::iterator it = TimerList.begin();
		for(;it!=TimerList.end();++it)
		{
			(*it)->Tickdown();
		}
	}

	void Gameplay::Spawn(float dt)
	{
		std::list<Spawner*>::iterator it = SpawnerList.begin();
		for(;it!=SpawnerList.end();++it)
		{
			(*it)->Fire(dt);
		}
	}

	void Gameplay::Trigger()
	{
		std::list<KrakEngine::Trigger*>::iterator it = TriggerList.begin();
		for(;it!=TriggerList.end();++it)
		{
			(*it)->Run();
		}	
	}


	void Gameplay::ParticleRun()
	{
		std::list<KrakEngine::Particle*>::iterator it = ParticleList.begin();
		for(;it!=ParticleList.end();++it)
		{
			(*it)->Run();

			if ((*it)->GetOwner()->GetType() == "car")
			{
				(*it)->setScript("");
			}
		}	
	}

	void Gameplay::ZoomOut()
	{ 
		zTarget = m_LevelDepth + GetCurrentTrack(); 
		zCamerachange = true;
		isZoomDirectionIn = false;
		zstep = 0;
	}

	void Gameplay::ZoomIn()
	{ 
		zTarget = m_LevelDepth + GetCurrentTrack(); 
		zCamerachange = true;
		isZoomDirectionIn = true;
		zstep = 0;
	}

	void Gameplay::AdjustYCamera()	{
		Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

		Camera::PanStatus pan = c->GetPan();
		if(pan == Camera::No_Pan) return;

		float targetheight = c->GetTargetHeight();
		float targetlookposition = c->GetTargetLookHeight();

		float currentheight = c->GetHeight();
		float currentlook = c->GetLookHeight();

		if(pan == Camera::Pan_Up)
		{
			if(currentheight < targetheight)
			{
				c->SetHeight(currentheight + 0.25f);
			}
			else
			{
				c->SetPan(Camera::No_Pan);
			}

			if (currentlook < targetlookposition)
			{
				c->SetLookHeight(currentlook + 0.25f);
			}
		}
		else
		{
			if(currentheight > targetheight)
			{
				c->SetHeight(currentheight - 0.25f);
			}	
			else
			{
				c->SetPan(Camera::No_Pan);
			}

			if (currentlook > targetlookposition)
			{
				c->SetLookHeight(currentlook - 0.25f);
			}
		}
	}

	void Gameplay::AdjustZCamera()
	{
		if(!zCamerachange) return;

		Camera * c = g_GRAPHICSSYSTEM->GetCurrentCamera();

		//To keep the camera from switching when a z collision determines the objects should not move
		if(!Player)
			return;

		float zdepth = c->GetDepth();

	  float v  = zstep / 300.0f;
	  v = v * v * (3 - 2 * v);
	  float step = (zTarget * v) + (zdepth * (1 - v));
 
		Transform *playert = Player->has(Transform);
		float curz = playert->GetPosition().z;
		if((prevz - curz > -0.001f &&  
			prevz - curz < 0.001f))
		{
			zCamerachange = false;
			return;
		}
		
		if(isZoomDirectionIn)
		{
			if (zdepth < zTarget)
				zdepth = step;
			else
				zCamerachange = false;
		}
		else
		{
			if (zdepth > zTarget)
				zdepth = step;		
			else
				zCamerachange = false;
		}

		c->SetDepth(zdepth);

		zstep++;
	}

	float Gameplay::GetCurrentTrack()
	{
		Transform * tc = Player->has(Transform);
		return tc->GetPosition().z;
	}

		
	void Gameplay::RemoveTimerComponent(int objectid)
	{
		std::list<Timer*>::iterator it = TimerList.begin();
		for(;it!=TimerList.end();++it)
		{
			if((*it)->GetOwnerID() == objectid)
			{
				TimerList.erase(it);
				return;	
			}
		}
	}

	void Gameplay::RemoveSpawnerComponent(int objectid)
	{
		std::list<Spawner*>::iterator it = SpawnerList.begin();
		for(;it!=SpawnerList.end();++it)
		{
			if((*it)->GetOwnerID() == objectid)
			{
				SpawnerList.erase(it);
				return;	
			}
		}
	}

	void Gameplay::RemoveTriggerComponent(int objectid)
	{
		std::list<KrakEngine::Trigger*>::iterator it = TriggerList.begin();
		for(;it!=TriggerList.end();++it)
		{
			if((*it)->GetOwnerID() == objectid)
			{
				TriggerList.erase(it);
				return;	
			}
		}
	}

	void Gameplay::RemoveParticleComponent(int objectid)
	{
		std::list<KrakEngine::Particle*>::iterator it = ParticleList.begin();
		for(;it!=ParticleList.end();++it)
		{
			if((*it)->GetOwnerID() == objectid)
			{
				ParticleList.erase(it);
				return;	
			}
		}
	}

	void Gameplay::EmplaceNewObjectOnLevelEditorList(std::string name)
	{
		if(!CurrentLevelEditor) return;

		CurrentLevelEditor->PlaceOnObjectTypeList(name);
	}
	
	
	void Gameplay::ResetCursor()
	{
		if(!Cursor) return;

		//Transform * tc = Cursor->has(Transform);
		Image * tc = Cursor->has(Image);
		if(tc)
		{			
			XMFLOAT3 pos = tc->GetPosition();
			pos.x = 0.5f;
			pos.y = 0.5f;

			tc->SetPosition(pos);
			/*
			XMFLOAT4X4 identity;
			XMStoreFloat4x4(&identity, XMMatrixIdentity());
			XMFLOAT2 screencoord = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(pos, identity);
			XMUINT2 screensize = g_GRAPHICSSYSTEM->GetWindowSize();
		
			if(screencoord.x > screensize.x || 
				screencoord.y > screensize.y || 
				screencoord.x < 0 ||
				screencoord.y < 0)
			{
				Transform * playertc = Player->has(Transform);
				XMFLOAT3 playerpos = playertc->GetPosition();
				playerpos.z = -6.1f;
				tc->SetPosition(playerpos);
			}
			*/
		}

	}

	void Gameplay::Jump()
	{        
		RigidBody * playerPhysics = Player->has(RigidBody);
		Audio * playerAudio = Player->has(Audio);
		SetAnimationState(AnimationStateJumping);
#if defined(_DEBUG)

			playerPhysics->velocity.y = 10.0f; //jump speed -->temp
			if(playerAudio)
				playerAudio->playJump();
#else
		if(CheatDoubleJump)
		{
			playerPhysics->velocity.y = 10.0f; //jump speed -->temp
				if(playerAudio)
					playerAudio->playJump();
		}
		else
		{

        if (playerPhysics->collision & COLLISION_BOTTOM){
				if(playerPhysics)
				{
					playerPhysics->velocity.y = 10.0f; //jump speed -->temp
					Audio * a = Player->has(Audio);
					if(a)
						playerAudio->playJump();
				}
			}
		}
#endif
	}

	void Gameplay::SetAnimationState(AnimationState as)
	{
		m_AnimationState = as;
		// Get the sprite of the character being controlled by the hat and set its direction
		Sprite* pSprite = Player->has(Sprite);
		if(pSprite) pSprite->SetAnimationState(as);
		// Get the sprite of the hat and set its direction
		pSprite = g_CONTROLLERSYSTEM->GetHat()->has(Sprite);
		if(pSprite) pSprite->SetAnimationState(as);
	}

	void Gameplay::SetIsPlayerFacingRight(bool isFacingRight){
		// Get the sprite of the character being controlled by the hat and set its direction
		Sprite* pSprite = Player->has(Sprite);
		if(pSprite) pSprite->SetIsFacingRight(isFacingRight);
		// Get the sprite of the hat and set its direction
		pSprite = g_CONTROLLERSYSTEM->GetHat()->has(Sprite);
		if(pSprite) pSprite->SetIsFacingRight(isFacingRight);
	}

	void Gameplay::LoadingInitial(){
		UnloadLevel();
		InitializeGame();
		
		g_GRAPHICSSYSTEM->HideTriggers();

		CORE->SetNextState(GameStates::InGame);
	}

	void Gameplay::LoadingRestart(){
		UnloadLevel();

		g_FACTORY->DeleteAllObjects();

		LoadLevelFile(CORE->GetCurrentLevel());
		
		g_GRAPHICSSYSTEM->HideTriggers();

		CORE->SetNextState(GameStates::InGame);
	}
}