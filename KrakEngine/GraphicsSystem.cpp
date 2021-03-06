/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GraphicsSystem.cpp
Purpose: System for Graphics Drawing
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 1/20/2014
- End Header -----------------------------------------------------*/

#include "Precompiled.h"
#include "GraphicsSystem.h"
#include "Camera.h"
#include "Sprite.h"
#include "Model.h"
#include "Text.h"
#include "Image.h"
#include "Trigger.h"
#include "GameObject.h"
#include "Transform.h"
#include "Factory.h"
#include "RigidBody.h"
#include "RBody.h"
#include "Conversions.h"
#include "Animation.h"
#include <winnt.h>

#include <iostream>

#include "TAM.h"
#include "DrawingState.h"
#include "ShaderResources.h"
#define D2D_ENABLED
#define TRACK_DEPTH 4.0f

namespace KrakEngine{
    
	GraphicsSystem* g_GRAPHICSSYSTEM = nullptr;

    DrawingState* g_DRAWSTATE = nullptr;

    GraphicsSystem::GraphicsSystem(WindowSystem* window) :
        m_Window(window),
        m_DPIX(96.0f),
        m_DPIY(96.0f),
        m_DrawDebug(0),
        m_ChooseModel(1),
        m_Viewport(0.0f, 0.0f, 800.0f, 600.0f, D3D11_MIN_DEPTH, D3D11_MAX_DEPTH),
        m_pCurrentCamera(NULL),
        m_IntermediateRTFormat(DXGI_FORMAT_R8G8B8A8_UNORM),
        m_bInitialized(false),
        m_bEditPath(false),
        m_bDoIK(false),
        m_NormalizedDistanceAlongArc(0.f),
        m_IKTargetPosition(-20.f, 10.f, 0.f),
        m_bInterpolateIK(false),
        m_IKAnimationTime(0.f)
    {
        DXThrowIfFailed(CoInitialize(NULL));

        //Set up the global pointer
        ThrowErrorIf(g_GRAPHICSSYSTEM != NULL, "Graphics already initialized");
		g_GRAPHICSSYSTEM = this;
        g_DRAWSTATE = new DrawingState();
	}

    GraphicsSystem::~GraphicsSystem(){
        CoUninitialize();
        if (g_DRAWSTATE)
        {
            delete g_DRAWSTATE;
        }
    }
    
    bool GraphicsSystem::Initialize(){
        // Hard coded path for model to follow initially
        for (int i = 0; i < 2; ++i)
        {
            m_PathControlPoints.push_back(Vector3((float)(i - 1) * -20.0f, m_GroundLevel, 0.f));
        }
        GenerateSplineInterpolationSystem();
        UpdateLinearSystem();

        LoadResources();
        m_bInitialized = true;
        return true;
    }
	bool GraphicsSystem::Shutdown(){
		printf("SHUTDOWN GRAPHICS\n");

		return true;
	}

    void GraphicsSystem::HandleMessages(Message* message){

        switch(message->MessageId)
        {
        case MId::ObjectDestroyed:
            {
                ObjectDestroyedMessage * pODM = dynamic_cast<ObjectDestroyedMessage *>(message);
			    RemoveModelComponent(pODM->GetID());
			    RemoveSpriteComponent(pODM->GetID());
                RemoveCameraComponent(pODM->GetID());
                RemoveTextComponent(pODM->GetID());
				RemoveImageComponent(pODM->GetID());        
                break;
            }
        case MId::ClearComponentLists:
            {
			/*	auto itm = ModelList.begin();
			for( ; itm != ModelList.end(); ++itm)
				delete (*itm);*/

			    m_ModelList.clear();

			//auto its = SpriteList.begin();
			//for( ; its != SpriteList.end(); ++its)
			//	delete (*its);

                m_SpriteList.clear();
                m_BackgroundSpriteList.clear();

			//auto itc = CameraList.begin();
			//for( ; itc != CameraList.end(); ++itc)
			//	delete (*itc);

                m_CameraList.clear();


				
	

			//auto ittext = TextList.begin();
			//for( ; ittext != TextList.end(); ++ittext)
			//	delete (*ittext);

                m_TextList.clear();

                m_ImageList.clear();
                break;
            }
        /* TODO: add this back once we can smoothly animate the camera from one position to another
        case MId::Collision:
            {
                CollisionMessage * pCM = dynamic_cast<CollisionMessage *>(message);
                if(pCM->GetObjectID() == m_pCurrentCamera->GetOwnerID() && (pCM->GetCollisionFlag() & COLLISION_BOTTOM) && (g_FACTORY->GetObjList().FindByID(pCM->GetColliderID())->GetType() != "mime" && g_FACTORY->GetObjList().FindByID(pCM->GetColliderID())->GetType() != "fireman"))
                {
                    Transform* pT = m_pCurrentCamera->GetOwner()->has(Transform);
                    m_pCurrentCamera->SetYOffset(pT->GetPosition().y);
                }
                break;
            }
        */
            case MId::Maximized:
            case MId::Restored:
			case MId::ResolutionChanged :
            case MId::ToggleFullScreen :
            {
                if(m_bInitialized){
                    // Make full screen
				    UpdateForWindowSizeChange();
                

                    ComPtr<IDXGIOutput> spOutput;
                    m_spSwapChain1->GetContainingOutput(&spOutput);
                    //m_spSwapChain1->SetFullscreenState(true, spOutput.Get());
                }
                break;                
            }
        default:
            break;
        }
	};

    void GraphicsSystem::Update(float dt){        
        m_spD3DDeviceContext1->ClearRenderTargetView(m_spD3DRenderTargetView.Get(), Colors::White);
        m_spD3DDeviceContext1->ClearRenderTargetView(m_spIntermediateRTV.Get(), Colors::White);
        m_spD3DDeviceContext1->ClearRenderTargetView(m_spIntermediateRTVDebug.Get(), Colors::White);
        m_spD3DDeviceContext1->ClearDepthStencilView(m_GBuffer.GetDepthStencilView(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
        m_spD3DDeviceContext1->OMSetDepthStencilState(m_GBuffer.GetDepthStencilState(), 1);

            
        m_spD2DDeviceContext->SetTarget(m_spD2DTargetBitmap.Get());
        
        // Set the blend state
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        UINT sampleMask   = 0xffffffff;

        m_spD3DDeviceContext1->OMSetBlendState(m_spBlendStateDisable.Get(), blendFactor, sampleMask);
        

        // Update the Camera
        // For now, we're only using one camera, so use the camera in the front of the list
        ThrowErrorIf(m_CameraList.size() == 0, "You must have a camera somewhere in your level file. Ideally as a component in the player object.");
        m_pCurrentCamera = m_CameraList.front();

        m_pCurrentCamera->Update(dt);

        g_DRAWSTATE->Update(dt);

        // Update the once per-frame constant buffer
        ConstantBufferPerFrame cbPerFrame;
        XMStoreFloat4x4(&(cbPerFrame.View), XMMatrixTranspose(XMLoadFloat4x4(m_CameraList.front()->GetView())));
        XMStoreFloat4x4(&(cbPerFrame.Projection), XMMatrixTranspose(XMLoadFloat4x4(&m_Projection)));
        cbPerFrame.ScreenSize.x = (float)m_WindowSize.x; //g_GRAPHICSSYSTEM->GetWindowSize().x;//m_WindowSize.x;
        cbPerFrame.ScreenSize.y = (float)m_WindowSize.y; //g_GRAPHICSSYSTEM->GetWindowSize().y;//m_WindowSize.y;
        cbPerFrame.LightPosition.x = g_DRAWSTATE->m_lightPosition[0];
        cbPerFrame.LightPosition.y = g_DRAWSTATE->m_lightPosition[1];
        cbPerFrame.LightPosition.z = g_DRAWSTATE->m_lightPosition[2];
        cbPerFrame.LightPosition.w = g_DRAWSTATE->m_lightPosition[3];
        cbPerFrame.StrokeRotation = g_DRAWSTATE->m_strokeDirectionTime;
        m_spD3DDeviceContext1->UpdateSubresource(m_spConstantBufferPerFrame.Get(), 0, nullptr, &cbPerFrame, 0, 0);

        // Setup the graphics pipeline. For now we use the same InputLayout and set of
        // constant buffers for all shaders, so they only need to be set once per frame.
        m_spD3DDeviceContext1->IASetInputLayout(m_spAlbedoModelVertexLayout.Get());
        m_spD3DDeviceContext1->VSSetConstantBuffers(0, 1, m_spConstantBufferPerFrame.GetAddressOf());
        m_spD3DDeviceContext1->VSSetConstantBuffers(1, 1, m_spConstantBufferPerObjectVS.GetAddressOf());
        
        m_spD3DDeviceContext1->PSSetConstantBuffers(0, 1, m_spConstantBufferPerFrame.GetAddressOf());

        UpdateAnimation(dt);
//        DrawFloor();
        //DrawBodies(g_PHYSICSSYSTEM->m_rigidBodies, NBODIES);
        UpdateModels(dt);
        if(true){//IsGBufferCreationOn()){            
            // Draw the models to the GBuffer
            m_GBuffer.TargetGBuffer(m_spD3DDeviceContext1);
            DrawModels();
            m_GBuffer.UnbindTargets(m_spD3DDeviceContext1);

            RenderLuminance(m_spLuminanceBufferRTV);

            if (g_DRAWSTATE->m_technique == Technique::Luminance)
            {
                RenderLuminanceGradient();
            }
            else
            {
                RenderPrincipalCurvatureDirection();
            }

            RenderStrokes();

            if(g_DRAWSTATE->m_drawingMode == DebugDrawingMode::GBufferNormal){
                // Target the back buffer
                m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), NULL);            
            
                // Bind the Resource Views
                m_GBuffer.BindInput(m_spD3DDeviceContext1, m_spPointSampler, m_spMirrorSampler);
                m_GBuffer.PrepareForUnpack(m_spD3DDeviceContext1, m_spConstantBufferGBufferUnpack, m_pCurrentCamera->GetView(), &m_Projection);
                
                // Render the GBuffer visualizer shaders
                DrawFullScreenQuad(m_spGBufferVisualizerVertexShader, m_spGBufferVisualizerPixelShader);

                // Cleanup
                UnbindGBuffer();
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::LuminanceBuffer)
            {
                DrawBufferRed(m_spLuminanceBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::XYDirectionBufferX)
            {
                DrawBufferRed(m_spXYDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::XYDirectionBufferY)
            {
                DrawBufferGreen(m_spXYDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::XYDirectionBuffer)
            {
                DrawBuffer(m_spXYDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::DirectionBuffer)
            {
                DrawBuffer(m_spStrokeDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::DirectionBufferX)
            {
                DrawBufferRed(m_spStrokeDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::DirectionBufferY)
            {
                DrawBufferGreen(m_spStrokeDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::DirectionBufferZ)
            {
                DrawBufferBlue(m_spStrokeDirectionBufferSRV);
            }
            if (g_DRAWSTATE->m_drawingMode == DebugDrawingMode::UniformDirection) {
//                RenderUniformDirection();
            }
            if(g_DRAWSTATE->m_drawingMode == DebugDrawingMode::EdgeDetection){//true){//IsSceneDrawingOn()){
                DoPostProcessing();
            }
        }

        UpdateSprites(dt);

        if (true) {
            // Set the render target to the back buffer and draw the sprites
            m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), m_GBuffer.GetReadOnlyDSV());

            m_spD3DDeviceContext1->OMSetBlendState(m_spBlendStateEnable.Get(), blendFactor, sampleMask);
            m_spD3DDeviceContext1->IASetInputLayout(m_spSpriteVertexLayout.Get());
            m_spD3DDeviceContext1->VSSetConstantBuffers(0, 1, m_spConstantBufferPerFrame.GetAddressOf());
            m_spD3DDeviceContext1->VSSetConstantBuffers(1, 1, m_spConstantBufferPerSpriteObject.GetAddressOf());

            m_spD3DDeviceContext1->PSSetConstantBuffers(0, 1, m_spConstantBufferPerFrame.GetAddressOf());
            m_spD3DDeviceContext1->PSSetConstantBuffers(1, 1, m_spConstantBufferPerSpriteObject.GetAddressOf());

            DrawSprites();
        }

#ifdef D2D_ENABLED
        // Draw FPS
        m_spD2DDeviceContext->BeginDraw();

        //DrawTAMs
        //get dimension of each TAM
        /*float tamHeight = m_spD2DDeviceContext->GetSize().height / (float)m_tamGenerator.m_imageTable.size();
        float tamWidth = m_spD2DDeviceContext->GetSize().width / (float)m_tamGenerator.m_imageTable[0].size();
        float tamDimension = min(tamHeight, tamWidth);
        for (size_t i = 0; i < m_tamGenerator.m_imageTable.size(); ++i)
        {
            for (size_t j = 0; j < m_tamGenerator.m_imageTable[i].size(); ++j)
            {
                m_spD2DDeviceContext->DrawBitmap(
                    m_tamGenerator.m_imageTable[i][j].Get(),
                    D2D1::RectF(
                        j * tamDimension,
                        i * tamDimension,
                        j * tamDimension + tamDimension,
                        i * tamDimension + tamDimension));
            }
        }*/


        // Create a text component for fps
		char buffer[32];
		ZeroMemory(buffer, sizeof(buffer));
//#if defined(_DEBUG)
        FramesPerSecond.push_back(dt);
        if(FramesPerSecond.size() > 60)
            FramesPerSecond.pop_front();

        float avgfps = 0.0f;
        float minfps = 0.0f;
        std::list<float>::iterator it = FramesPerSecond.begin();
        for(; it!=FramesPerSecond.end(); ++it){
            avgfps += *it;
            // Find the maximum frame time
            minfps = max(minfps, *it);
        }

        avgfps = (float)FramesPerSecond.size() / avgfps;
        minfps = 1.0f / minfps;

        char avgbuffer[32];
        _itoa_s((int)avgfps, avgbuffer, 32, 10);

        char minbuffer[32];
        _itoa_s((int)minfps, minbuffer, 32, 10);

        ThrowErrorIf(m_TextList.empty(), "Must have at least one text component in the level for FPS");
        std::string str("Avg FPS: ");
        str.append(avgbuffer);
        //str.append("\rMin Framerate: ");
        //str.append(minbuffer);
        str.append("\rPress m to cycle model\r");
        std::string debugModeString = g_DRAWSTATE->GetDebugModeString();
        str.append(debugModeString);
		std::list<Text*>::iterator textit = m_TextList.begin();
        for(;textit!=m_TextList.end();++textit)
        {
			if((*textit)->GetOwner()->GetName() == "FPS")
			{
				(*textit)->SetTextContent(str);
				break;
			}
		}
/*#else
		std::list<Text*>::iterator textit = m_TextList.begin();
        for(;textit!=m_TextList.end();++textit)
        {
			if((*textit)->GetOwner()->GetName() == "FPS")
			{
				(*textit)->SetTextContent(buffer);
				break;
			}
		}
#endif*/
        //UpdateImages(dt);
        //DrawImages();
        
        SortText();
        DrawTextComponents();
        
       

        DXThrowIfFailed(
            m_spD2DDeviceContext->EndDraw()
            );
#endif
        // Draw debug shapes
        /*if (IsDebugDrawingOn())
        {
            DrawDebugInfo();
        }*/
        // DrawBones();

        //DrawPathMidpoint();
        //DrawPathSplineInterpolation();
        //DrawIKTarget();
        if (m_bEditPath)
        {
            DrawMouse();
        }
        
		DrawSelected();

		TwDraw();

        DXThrowIfFailed(
            m_spSwapChain1->Present(0, 0)
            );
        // Discard the contents of the render target.
        // This is a valid operation only when the existing contents will be entirely
        // overwritten. If dirty or scroll rects are used, this call should be removed.
        m_spD3DDeviceContext1->DiscardView(m_spD3DRenderTargetView.Get());
        m_spD3DDeviceContext1->DiscardView(m_spIntermediateRTV.Get());
        m_spD3DDeviceContext1->DiscardView(m_spIntermediateRTVDebug.Get());
        // Discard the contents of the GBuffer and Depth Stencil
        //m_spD3DDeviceContext1->DiscardView(m_spD3DDepthStencilView.Get());
        //m_GBuffer.DiscardViews(m_spD3DDeviceContext1);
    }

    void GraphicsSystem::BindGBufferAsInput()
    {
        // Bind the GBuffer as input
        m_GBuffer.BindInput(m_spD3DDeviceContext1, m_spPointSampler, m_spMirrorSampler);

        // Prepare the GBuffer to be unpacked
        m_GBuffer.PrepareForUnpack(m_spD3DDeviceContext1, m_spConstantBufferGBufferUnpack, m_pCurrentCamera->GetView(), &m_Projection);
    }

    void GraphicsSystem::UnbindGBuffer()
    {
        m_GBuffer.UnbindInput(m_spD3DDeviceContext1);
    }

    void GraphicsSystem::BindPencilShaderInput()
    {
    }

    void GraphicsSystem::RenderLuminance(ComPtr<ID3D11RenderTargetView> spRTV){
        m_spD3DDeviceContext1->OMSetRenderTargets(1, spRTV.GetAddressOf(), nullptr);

        BindGBufferAsInput();

        // Render the directional lights
        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spLuminanceVertexShader, m_spLuminancePixelShader);

        // Unbind input
        UnbindGBuffer();
    }

    void GraphicsSystem::DrawBufferSetup(ComPtr<ID3D11ShaderResourceView> &buffer)
    {
        // Target Back Buffer
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), nullptr);

        // Bind Shader Resources
        ShaderResources resources(
        {}, // Vertex Shader Textures
        {}, // Vertex Shader Samplers
        { buffer.Get() }, // Pixel Shader Textures
        { m_spPointSampler.Get() } // Pixel Shader Samplers
        );
        BindShaderResources(m_spD3DDeviceContext1, resources);
    }

    void GraphicsSystem::DrawBufferTeardown(ComPtr<ID3D11ShaderResourceView> &buffer)
    {
        // Should match resources in DrawBufferSetup
        ShaderResources resources(
        {}, // Vertex Shader Textures
        {}, // Vertex Shader Samplers
        { buffer.Get() }, // Pixel Shader Textures
        { m_spPointSampler.Get() } // Pixel Shader Samplers
        );
        UnBindShaderResources(m_spD3DDeviceContext1, resources);
    }

    void GraphicsSystem::DrawBuffer(ComPtr<ID3D11ShaderResourceView> &buffer)
    {
        DrawBufferSetup(buffer);
        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spBufferVisualizerPixelShader);
        
        DrawBufferTeardown(buffer);
    }

    void GraphicsSystem::DrawBufferRed(ComPtr<ID3D11ShaderResourceView> &buffer)
    {
        DrawBufferSetup(buffer);
        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spBufferVisualizerRedPixelShader);

        DrawBufferTeardown(buffer);
    }

    void GraphicsSystem::DrawBufferGreen(ComPtr<ID3D11ShaderResourceView> &buffer)
    {
        DrawBufferSetup(buffer);
        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spBufferVisualizerGreenPixelShader);

        DrawBufferTeardown(buffer);
    }

    void GraphicsSystem::DrawBufferBlue(ComPtr<ID3D11ShaderResourceView> &buffer)
    {
        DrawBufferSetup(buffer);
        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spBufferVisualizerBluePixelShader);

        DrawBufferTeardown(buffer);
    }

    void GraphicsSystem::RenderLuminanceGradient() {
        // Down-res the luminance buffer
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spQuarterResBufferRTV.GetAddressOf(), nullptr);

        ShaderResources shaderResources(
        {}, // Vertex Shader Textures
        {}, // Vertex Shader Samplers
        { m_spLuminanceBufferSRV.Get() }, // Pixel Shader Textures
        { m_spPointSampler.Get(), m_spMirrorSampler.Get() } // Pixel Shader Samplers
        );

        // Bind Shader Resources        
        BindShaderResources(m_spD3DDeviceContext1, shaderResources);

        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spDownSampleVertexShader, m_spQuarterResDownSamplePixelShader);
        
        UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);

        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spQuarterResBufferRTV2.GetAddressOf(), nullptr);

        // Bind Shader Resources
        shaderResources = ShaderResources(
        {}, // Vertex Shader Textures
        {}, // Vertex Shader Samplers
        { m_spQuarterResBufferSRV.Get() }, // Pixel Shader Textures
        { m_spPointSampler.Get(), m_spMirrorSampler.Get() } // Pixel Shader Samplers
        );
        BindShaderResources(m_spD3DDeviceContext1, shaderResources);

        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spHorizontalSobelPixelShader);

        UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);

        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spQuarterResBufferRTV.GetAddressOf(), nullptr);

        // Bind Shader Resources
        shaderResources = ShaderResources(
        {}, // Vertex Shader Textures
        {}, // Vertex Shader Samplers
        { m_spQuarterResBufferSRV2.Get() }, // Pixel Shader Textures
        { m_spPointSampler.Get(), m_spMirrorSampler.Get() } // Pixel Shader Samplers
        );
        BindShaderResources(m_spD3DDeviceContext1, shaderResources);

        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spVerticalSobelPixelShader);

        UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);
        //=============================================
        // Two pass Gaussian Blur
        //=============================================
        if(g_DRAWSTATE->m_isGradientBufferBlurOn)
        {
            for (int i = 0; i < g_DRAWSTATE->m_numBlurPasses; ++i)
            {
                m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spQuarterResBufferRTV2.GetAddressOf(), nullptr);
                // Bind Shader Resources
                shaderResources = ShaderResources(
                {}, // Vertex Shader Textures
                {}, // Vertex Shader Samplers
                { m_spQuarterResBufferSRV.Get() }, // Pixel Shader Textures
                { m_spPointSampler.Get(), m_spMirrorSampler.Get() } // Pixel Shader Samplers
                );
                BindShaderResources(m_spD3DDeviceContext1, shaderResources);

                // Draw a fullscreen quad with the output
                DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spHorizontalGaussianBlurPixelShader);

                UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);

                m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spQuarterResBufferRTV.GetAddressOf(), nullptr);

                // Bind Shader Resources
                shaderResources = ShaderResources(
                {}, // Vertex Shader Textures
                {}, // Vertex Shader Samplers
                { m_spQuarterResBufferSRV2.Get() }, // Pixel Shader Textures
                { m_spPointSampler.Get(), m_spMirrorSampler.Get() } // Pixel Shader Samplers
                );
                BindShaderResources(m_spD3DDeviceContext1, shaderResources);

                // Draw a fullscreen quad with the output
                DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spVerticalGaussianBlurPixelShader);

                UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);
            }
        }

        // UpRes the resulting gradient texture
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spXYDirectionBufferRTV.GetAddressOf(), nullptr);
        shaderResources = ShaderResources(
        {}, // Vertex Shader Textures
        {}, // Vertex Shader Samplers
        { m_spQuarterResBufferSRV.Get() }, // Pixel Shader Textures
        { m_spPointSampler.Get(), m_spMirrorSampler.Get() }); // Pixel Shader Samplers

        // Bind Shader Resources
        BindShaderResources(m_spD3DDeviceContext1, shaderResources);

        // Draw a fullscreen quad with the output
        DrawFullScreenQuad(m_spDownSampleVertexShader, m_spQuarterResUpSamplePixelShader);

        UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);

        ConvertPerPixelXYDirectionBufferToPerVertexDirectionBuffer();
    }

    void GraphicsSystem::ConvertPerPixelXYDirectionBufferToPerVertexDirectionBuffer()
    {
        // Render the stroke directions based on the gradient buffer
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spStrokeDirectionBufferRTV.GetAddressOf(), nullptr);

        // Bind Shader Resources
        ShaderResources shaderResources(
        { m_spXYDirectionBufferSRV.Get() }, // Vertex Shader Textures
        { m_spPointSampler.Get(), }, // Vertex Shader Samplers
        {}, // Pixel Shader Textures
        {} // Pixel Shader Samplers
        );

        BindShaderResources(m_spD3DDeviceContext1, shaderResources);

        // Draw the models and output the stroke direction buffer
        DrawModels(m_spPerPixelToPerVertexStrokeDirectionVertexShader, m_spPerPixelToPerVertexStrokeDirectionPixelShader, shaderResources);

        UnBindShaderResources(m_spD3DDeviceContext1, shaderResources);
    }

    void GraphicsSystem::RenderPrincipalCurvatureDirection()
    {
        // Target the stroke direction render target
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spXYDirectionBufferRTV.GetAddressOf(), nullptr);
        
        BindGBufferAsInput();

        DrawFullScreenQuad(m_spPrincipalCurvatureStrokeDirectionVertexShader, m_spPrincipalCurvatureStrokeDirectionPixelShader);

        UnbindGBuffer();

        ConvertPerPixelXYDirectionBufferToPerVertexDirectionBuffer();
    }

    void GraphicsSystem::RenderStrokes()
    {
        // Target the back buffer
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), NULL);

        // Bind the Resource Views
        //BindPencilShaderInput();
        ShaderResources resources(
        {},
        {},
        {   m_spStrokeDirectionBufferSRV.Get(),
            m_spLuminanceBufferSRV.Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade0").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade1").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade2").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade3").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade4").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade5").Get() },
         { m_spPointSampler.Get(), m_spMirrorSampler.Get() });

        BindShaderResources(m_spD3DDeviceContext1, resources);

        // Render the Strokes
        DrawFullScreenQuad(m_spBufferVisualizerVertexShader, m_spRenderStrokesPixelShader);

        // Unbind the resources
        UnBindShaderResources(m_spD3DDeviceContext1, resources);
    }

    /*void GraphicsSystem::RenderUniformDirection()
    {
        // Target the back buffer
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), NULL);

        // Bind the Resource Views
        //BindPencilShaderInput();
        ShaderResources resources(
            { m_spXYDirectionBufferSRV.Get()},
            { m_spPointSampler.Get(), m_spMirrorSampler.Get() },
            { m_spXYDirectionBufferSRV.Get(),
            m_spLuminanceBufferSRV.Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade0").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade1").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade2").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade3").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade4").Get(),
            g_GRAPHICSSYSTEM->GetTexture("shade5").Get() },
            { m_spPointSampler.Get(), m_spMirrorSampler.Get() });
        BindShaderResources(m_spD3DDeviceContext1, resources);

        // Render the GBuffer visualizer shaders
        DrawModels(m_spStrokeDirectionUniformVertexShader, m_spStrokeDirectionUniformPixelShader, resources);
    
        // Unbind the resources
        UnBindShaderResources(m_spD3DDeviceContext1, resources);
    }*/
    void GraphicsSystem::DoPostProcessing(){
        // Target an intermediate Render Target
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spIntermediateRTV.GetAddressOf(), NULL);
        //m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), NULL);

        // Bind the GBuffer as input
        m_GBuffer.BindInput(m_spD3DDeviceContext1, m_spPointSampler, m_spMirrorSampler);
        // Prepare the GBuffer to be unpacked
        m_GBuffer.PrepareForUnpack(m_spD3DDeviceContext1, m_spConstantBufferGBufferUnpack, m_pCurrentCamera->GetView(), &m_Projection);
        
        // Draw the fullscreen quad
        DrawFullScreenQuad(m_spContourDetectionPass1VertexShader, m_spContourDetectionPass1PixelShader);

        // Unbind the GBuffer as input
        m_GBuffer.UnbindInput(m_spD3DDeviceContext1);
        
        // Target the back buffer
        m_spD3DDeviceContext1->OMSetRenderTargets(1, m_spD3DRenderTargetView.GetAddressOf(), NULL);    

        // Set the blend state        
        float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        UINT sampleMask   = 0xffffffff;
        m_spD3DDeviceContext1->OMSetBlendState(m_spBlendStateEnable.Get(), blendFactor, sampleMask);

        // Bind the first edge detection pass as input
        ID3D11ShaderResourceView* views[1] = {m_spIntermediateSRV.Get()};
        m_spD3DDeviceContext1->PSSetShaderResources(0, ARRAYSIZE(views), views);
        m_spD3DDeviceContext1->PSSetSamplers( 0, 1, m_spPointSampler.GetAddressOf() );

        // Draw the fullscreen quad
        DrawFullScreenQuad(m_spContourDetectionPass2VertexShader, m_spContourDetectionPass2PixelShader);

        // Unbind input
        ZeroMemory(views, sizeof(views));
        m_spD3DDeviceContext1->PSSetShaderResources(0, ARRAYSIZE(views), views);
    }

    void GraphicsSystem::DrawFullScreenQuad(ComPtr<ID3D11VertexShader> spVertexShader, ComPtr<ID3D11PixelShader> spPixelShader){
        D3D11_VIEWPORT oldViewPort[1];
        unsigned int numberOfViewports = 1;
        m_spD3DDeviceContext1->RSGetViewports(&numberOfViewports, oldViewPort);
        
        // Setup the Quad Viewport
        D3D11_VIEWPORT viewPort = {0};
        viewPort.Width =  (float)m_WindowSize.x;
        viewPort.Height = (float)m_WindowSize.y;
        viewPort.TopLeftX = 0.0f;
        viewPort.TopLeftY = 0.0f;
        m_spD3DDeviceContext1->RSSetViewports(1, &viewPort);

        // Set FullScreenQuad data
        m_FullScreenQuadStride = sizeof(FullScreenQuadVertex);
        m_FullScreenQuadOffset = 0;
                
        // Set input layout
        m_spD3DDeviceContext1->IASetInputLayout(m_spFullScreenQuadVertexLayout.Get());

        m_spD3DDeviceContext1->IASetVertexBuffers(0, 1, m_spFullScreenQuadVertexBuffer.GetAddressOf(), &m_FullScreenQuadStride, &m_FullScreenQuadOffset);
        
        // Set primitive topology
        m_spD3DDeviceContext1->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

        // Set the shaders
        m_spD3DDeviceContext1->VSSetShader(spVertexShader.Get(), NULL, 0);
        m_spD3DDeviceContext1->GSSetShader(NULL, NULL, 0);
        m_spD3DDeviceContext1->PSSetShader(spPixelShader.Get(), NULL, 0);
        m_spD3DDeviceContext1->Draw(4, 0);

        // Restore the old viewport
        m_spD3DDeviceContext1->RSSetViewports(numberOfViewports, oldViewPort);
    }

    void GraphicsSystem::CreateQuadResources()
    {
        // Create vertex buffer
	    SpriteVertex vertices[] =
        {
            { XMFLOAT3(-0.5f,  0.5f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // Blue (top left)
            { XMFLOAT3( 0.5f,  0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, // Green (top right)
            { XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, // Pink (bottom left)
            { XMFLOAT3( 0.5f, -0.5f, 0.0f), XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // Yellow (bottom right)
	    };
	    D3D11_BUFFER_DESC bd;
	    ZeroMemory(&bd, sizeof(bd));
	    bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(SpriteVertex) * ARRAYSIZE(vertices);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	    bd.CPUAccessFlags = 0;

	    D3D11_SUBRESOURCE_DATA InitData;
	    ZeroMemory(&InitData, sizeof(InitData));
	    InitData.pSysMem = vertices;
	    DXThrowIfFailed(
		    m_spD3DDevice1->CreateBuffer(
			    &bd, 
			    &InitData, 
                m_spQuadVertexBuffer.GetAddressOf()));

	    // Set vertex buffer
	    //UINT stride = sizeof(SpriteVertex);	Cristina 20140324 Unused variable
	    //UINT offset = 0;						Cristina 20140324 Unused variable
       
	    // Create index buffer
	    WORD indices[] =
	    {
            // Quad
		    0, 3, 2,
		    1, 3, 0,
	    };
	    bd.Usage = D3D11_USAGE_DEFAULT;
	    bd.ByteWidth = sizeof(WORD)* ARRAYSIZE(indices);        // 36 vertices needed for 12 triangles in a triangle list
	    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	    bd.CPUAccessFlags = 0;
	    InitData.pSysMem = indices;
	    DXThrowIfFailed(
		    m_spD3DDevice1->CreateBuffer(&bd, &InitData, m_spQuadIndexBuffer.ReleaseAndGetAddressOf()));
    }

    void GraphicsSystem::UpdateModels(float dt){
        // Draw each object
        std::list<Model*>::iterator it = m_ModelList.begin();
        for(;it!=m_ModelList.end();++it)
        {
            (*it)->Update(dt);
        }
    }

    void GraphicsSystem::DrawModels(){
        // Draw each object
        std::list<Model*>::iterator it = m_ModelList.begin();
        UINT ModelNumber = 0;
        for(;it!=m_ModelList.end();++it)
        {
            if (g_DRAWSTATE->m_isSkinningOn && (*it)->m_Controller != nullptr)
            {
                (*it)->Skin(m_spD3DDevice1, m_spD3DDeviceContext1);
            }
            if (/*ModelNumber < 1 ||*/ (ModelNumber == (UINT)m_ChooseModel) ){
                (*it)->Draw(m_spD3DDeviceContext1, m_spConstantBufferPerObjectVS, m_spConstantBufferPerObjectPS);
            }

            ++ModelNumber;
        }
    }

    void GraphicsSystem::DrawModels(const ComPtr<ID3D11VertexShader> &spVertexShader, const ComPtr<ID3D11PixelShader> &spPixelShader, ShaderResources &resources) {
        // Draw each object
        std::list<Model*>::iterator it = m_ModelList.begin();
        UINT ModelNumber = 0;
        for (; it != m_ModelList.end(); ++it)
        {
            if (g_DRAWSTATE->m_isSkinningOn && (*it)->m_Controller != nullptr)
            {
                (*it)->Skin(m_spD3DDevice1, m_spD3DDeviceContext1);
            }
            if (/*ModelNumber < 1 ||*/ (ModelNumber == (UINT)m_ChooseModel)) {
                (*it)->DrawShader(m_spD3DDeviceContext1, m_spConstantBufferPerObjectVS, m_spConstantBufferPerObjectPS, spVertexShader, spPixelShader, resources);
            }

            ++ModelNumber;
        }
    }

    void GraphicsSystem::DrawBones(){
        m_spD2DDeviceContext->BeginDraw();
        // Draw each object
        std::list<Model*>::iterator it = m_ModelList.begin();
        UINT ModelNumber = 0;
        for (; it != m_ModelList.end(); ++it)
        {
            if (ModelNumber == (UINT)m_ChooseModel){
                (*it)->DrawBones(m_spD2DDeviceContext, m_spD2DFactory, m_spConstantBufferPerObjectVS, m_spConstantBufferPerObjectPS);
            }

            ++ModelNumber;
        }
        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    float SmoothStep(float t)
    {
        return t * t * (3 - 2 * t);
    }

    void GraphicsSystem::InterpolateIK(float dt, AnimationController* pController)
    {
        m_IKAnimationTime += dt / m_IKAnimationLength;
        if (m_IKAnimationTime > 1.f)
        {
            m_IKAnimationTime = 1.f;
            m_bInterpolateIK = false;           
        }

        // Interpolate the rotations
        for (size_t i = 0; i < pController->m_pSkeleton->m_CurrentJointRotations2D.size(); ++i)
        {
            pController->m_pSkeleton->m_CurrentJointRotations2D[i] = Interpolate(
                pController->m_pSkeleton->m_OldJointRotations2D[i],
                pController->m_pSkeleton->m_TargetJointRotations2D[i],
                m_IKAnimationTime);
        }

        // Set the Old positions to the current positions once the animation is complete
        if (m_IKAnimationTime >= 1.f)
        {
            m_bInterpolateIK = false; // Mark the interpolation as complete
            pController->m_pSkeleton->m_OldJointPositions2D = pController->m_pSkeleton->m_CurrentJointPositions2D;
            pController->m_pSkeleton->m_OldJointRotations2D = pController->m_pSkeleton->m_CurrentJointRotations2D;
        }
    }

    void GraphicsSystem::UpdateAnimation(float dt)
    {
        m_NormalizedDistanceAlongArc += dt / m_AnimationLength;
        if (m_NormalizedDistanceAlongArc > 1.f)
        {
            m_NormalizedDistanceAlongArc = 1.f;
            m_bDoIK = true;
        }
        //else
        {
        
            // Use smoothstep to have an acceleration and deceleration at the beginning and the end
            float smoothDistanceAlongArc = SmoothStep(m_NormalizedDistanceAlongArc);
        
            // Get the time u along the parametric curve using the inverse arc length function
            // and the normalized distance along the arc the model should have traveled at this point
            float u = (float)InverseArcLength(smoothDistanceAlongArc);

            if (m_ModelList.size() > 1)
            {
                std::list<Model*>::iterator it = m_ModelList.begin();
                ++it;
                // For now, hard code the 2nd model to be the one walking
                if ((*it)->GetOwner())
                {
                    Transform* t = (*it)->GetOwner()->has(Transform);

                    // Store the old position for later use determining speed
                    XMFLOAT3 oldPos = t->GetPosition();
                    XMFLOAT2 oldPos2D = XMFLOAT2(oldPos.x, oldPos.z); // The floor is along the x and z planes

                    // Set the new position along the path
                    XMFLOAT2 pos2D = SplineInterpolate(u);
                    XMFLOAT3 pos3D = XMFLOAT3(pos2D.x, 0.0f, pos2D.y);
                    t->SetPosition(pos3D);
                    (*it)->m_Controller->m_pSkeleton->m_OldJointPositions2D[0] = XMFLOAT2(pos3D.x, pos3D.y);
                    (*it)->m_Controller->m_pSkeleton->m_CurrentJointPositions2D[0] = XMFLOAT2(pos3D.x, pos3D.y);
                    (*it)->m_Controller->m_pSkeleton->m_TargetJointPositions2D[0] = XMFLOAT2(pos3D.x, pos3D.y);
                    if (m_NormalizedDistanceAlongArc >= 1.f)
                    {
                        if (m_bInterpolateIK)
                        {
                            InterpolateIK(dt, (*it)->m_Controller);
                        }
                        else
                        {
                            XMFLOAT2 ikTargetPosition2D = XMFLOAT2(m_IKTargetPosition.x, m_IKTargetPosition.y);
                            bool complete = (*it)->m_Controller->ProcessIK2D(pos2D, ikTargetPosition2D);

                            if (complete)
                            {
                                // If the IK finished, but the end effector did not reach the destination
                                if (Mag((*it)->m_Controller->m_pSkeleton->m_TargetEndEffectorPosition2D - ikTargetPosition2D) > (*it)->m_Controller->m_pSkeleton->m_IKEpsilon)
                                {
                                    // Set a new path
                                    // Start at current position
                                    m_PathControlPoints.clear();
                                    m_PathControlPoints.push_back(pos3D);

                                    // Get direction towards target
                                    float xDir = m_IKTargetPosition.x - pos3D.x;
                                    float zDir = m_IKTargetPosition.z - pos3D.z;
                                    if (xDir != 0)
                                    {
                                        xDir = xDir / abs(xDir);
                                    }
                                    if (zDir != 0)
                                    {
                                        zDir = zDir / abs(zDir);
                                    }

                                    // Pick a point a little far away from the target so it can be reached.
                                    XMFLOAT3 dest = XMFLOAT3(m_IKTargetPosition.x - xDir * 20.f, m_GroundLevel, m_IKTargetPosition.z /*- zDir * 10.f*/);
                                    m_PathControlPoints.push_back(dest);

                                    if (Mag(dest - pos3D) > 1.f)
                                    {
                                        // Start the path over
                                        m_NormalizedDistanceAlongArc = 0.f;
                                        UpdateLinearSystem();
                                    }
                                }
                                else
                                {
                                    m_bInterpolateIK = true;
                                    m_IKAnimationTime = 0.f;
                                }
                            }
                        }
                    }

                    // Set the speed of the model animation based on how far the model has moved
                    (*it)->m_Controller->m_AnimationSpeed = m_StepSizeFactor * Mag(oldPos2D - pos2D) / dt;
                    (*it)->m_Controller->SetRootPos(pos3D);

                    // Center of interest approach to orientation
                    float coiTime = u + m_coiDelta;
                    if (coiTime > 1.f)
                    {
                        coiTime = 1.f;
                    }

                    // Position to look to
                    XMFLOAT2 coiPos = SplineInterpolate(coiTime);

                    // Get the direction between the current position and the center of interest
                    XMFLOAT2 lookDirection;
                    lookDirection.x = coiPos.x - pos2D.x;
                    lookDirection.y = coiPos.y - pos2D.y; // The XMFLOAT2 produced by SplineInterpolate is actually the x and z position

                    float lookAngle = AngleInDegrees(lookDirection, Vector2(0.f, -1.f));

                    if (lookDirection.x > 0){ lookAngle *= -1.f; }

                    // Set the rotation about the y axis
                    XMFLOAT3 rot = t->GetRotation();
                    rot.y = lookAngle;
                    t->SetRotation(rot);
                }
            }
        }
    }

    void GraphicsSystem::GenerateSplineInterpolationSystem()
    {
        // System used to solve for c0 + c1 * t + c2 * t^2 + c3 * t^3 + c4 (t - 1)^3+ ...
        m_LinearSystem = std::vector<std::vector<double> >(m_MaximumDegree + 3);
        for (size_t i = 0; i < m_LinearSystem.size(); ++i)
        {
            m_LinearSystem[i] = std::vector<double>(m_MaximumDegree + 3);
        }
        m_LinearSystemDoublePrimed = std::vector<std::vector<double> >(m_MaximumDegree + 3);
        for (size_t i = 0; i < m_LinearSystemDoublePrimed.size(); ++i)
        {
            m_LinearSystemDoublePrimed[i] = std::vector<double>(m_MaximumDegree + 3);
        }

        // i => t = 0, t = 1, ..., t = max degree + 3
        for (int i = 0; i < m_MaximumDegree + 3; ++i)
        {
            for (int j = 0; j < m_MaximumDegree + 3; ++j)
            {
                // c0 + c1 * t + c2 * t^2 + c3 * t^3
                if (j < 4)
                {
                    m_LinearSystem[i][j] = pow(i, j);

                    if (j < 2)
                    {
                        // 2nd derivative for c0 + c1 * t
                        m_LinearSystemDoublePrimed[i][j] = 0;
                    }
                    else if (j == 2)
                    {
                        // 2nd derivative for c2 * t^2
                        m_LinearSystemDoublePrimed[i][j] = 2;
                    }
                    else
                    {
                        // 2nd derivative for c3 * t^3
                        m_LinearSystemDoublePrimed[i][j] = 6 * i;
                    }
                }
                // c4 (t - 1)^3+ + c5 (t-2)^3+ + c6 (t-3)^3+ ...
                else
                {
                    m_LinearSystem[i][j] = pow(i - (j - 3), 3);
                    m_LinearSystemDoublePrimed[i][j] = 6 * (i - (j - 3));
                }

                // if t < c for (t-c)^3+
                if (j - i >= 3)
                {
                    m_LinearSystem[i][j] = 0;
                    m_LinearSystemDoublePrimed[i][j] = 0;
                }
            }
        }
    }

    // Takes in a vector of points and returns a list of the x-coordinates
    std::vector<double> XCoordinateList(std::vector<XMFLOAT3> points)
    {
        std::vector<double> xlist;
        for (size_t i = 0; i < points.size(); ++i)
        {
            xlist.push_back(points[i].x);
        }

        return xlist;
    }

    // Takes in a list of points and returns a list of the y-coordinates
    std::vector<double> ZCoordinateList(std::vector<XMFLOAT3> points)
    {
        std::vector<double> zlist;
        for (size_t i = 0; i < points.size(); ++i)
        {
            zlist.push_back(points[i].z);
        }

        return zlist;
    }

    std::vector<double> GraphicsSystem::BuildAndSolveLinearSystem(std::vector<double> coordlist)
    {
        // Copy list of coordinates to the array
        size_t n = coordlist.size() + 2;
        //std::vector<double> coords(n);
        //coords.resize(n);
        alglib::real_1d_array coords;
        coords.setlength(n);
        //std::vector<double> constants(n - 1);
        //constants.resize(n - 1);
        alglib::real_1d_array constants;
        constants.setlength(n - 1);

        for (size_t i = 0; i < n - 2; ++i)
        {
            coords[i] = coordlist[i];
        }
        // The last two values (for x'') are 0
        coords[n - 2] = 0;
        coords[n - 1] = 0;

        /*std::vector<std::vector<double> > A;
        for (size_t i = 0; i < n; ++i)
        {
            A.push_back(std::vector<double>(n));
        }*/

        alglib::real_2d_array a;
        a.setlength(n, n);

        for (size_t i = 0; i < n; ++i)
        {
            for (size_t j = 0; j < n; ++j)
            {
                a[i][j] = m_LinearSystem[i][j];
            }
        }

        // Determine the last two rows of A
        for (size_t i = 0; i < n; ++i)
        {
            a[n - 2][i] = m_LinearSystemDoublePrimed[0][i];

            a[n - 1][i] = m_LinearSystemDoublePrimed[n - 3][i];
        }
        alglib::ae_int_t  info = 0;
        alglib::densesolverreport report;
        
        alglib::rmatrixsolve(a, n, coords, info, report, constants);

        std::vector<double> out;
        for (int i = 0; i < constants.length(); ++i)
        {
            out.push_back(constants[i]);
        }
        return out;
    }

    // Calculate c0 + c1 * t + c2 * t^2 + c3 * t^3 + c4 (t - 1)^3+ + c5 (t-2)^3+ + c6 (t-3)^3+ ...
    double GraphicsSystem::Spline(std::vector<double> constants, double t)
    {
        double sum = 0;
        for (int i = 0; i < m_PathControlPoints.size() + 2; ++i)
        {
            // c0 + c1 * t + c2 * t^2 + c3 * t^3
            if (i < 4)
            {
                sum += constants[i] * pow(t, i);
            }
            // c4 (t - 1)^3+ + c5 (t-2)^3+ + c6 (t-3)^3+ ...
            else
            {
                if (t >= i - 3)
                {
                    sum += constants[i] * pow(t - (double)(i - 3), 3);
                }
            }
        }
        return sum;
    }

    XMFLOAT2 GraphicsSystem::SplineInterpolate(double t)
    {
        return XMFLOAT2((float)Spline(m_XConst, t * m_SplineScale), (float)Spline(m_ZConst, t * m_SplineScale));
    }

    void GraphicsSystem::UpdateIKSystem()
    {
        // Do IK

        // If target out of reach

        // New path to target
    }

    void GraphicsSystem::UpdateLinearSystem()
    {
        m_XConst = BuildAndSolveLinearSystem(XCoordinateList(m_PathControlPoints));
        m_ZConst = BuildAndSolveLinearSystem(ZCoordinateList(m_PathControlPoints));

        m_SplineScale = (double)(m_PathControlPoints.size() - 1);

        BuildArcLengthTable();
        NormalizeArcLengthTable();
    }

    void GraphicsSystem::DrawPathSplineInterpolation(){
        m_spD2DDeviceContext->BeginDraw();
        // Draw each path point
        std::vector<XMFLOAT3>::iterator it1 = m_PathControlPoints.begin();



        for (; it1 != m_PathControlPoints.end(); ++it1)
        {

            // Draw current path point
            // Convert from world to screen coordinates
            XMFLOAT2 points[2];
            points[0] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(*it1, m_World);

            m_spD2DDeviceContext->DrawEllipse(
                D2D1::Ellipse(
                D2D1::Point2F(points[0].x, points[0].y),
                m_PathPointRadius, m_PathPointRadius),
                GetD2DBrush(ColorBlue).Get());

            // Draw line from current point to next point
            std::vector<Vector3>::iterator it2 = it1;
            ++it2;
            if (it2 != m_PathControlPoints.end())
            {
                points[1] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(*it2, m_World);

                // Use D2D to draw a line in screen coordinates
                m_spD2DDeviceContext->DrawLine(
                    D2D1::Point2F(points[0].x, points[0].y),
                    D2D1::Point2F(points[1].x, points[1].y),
                    GetD2DBrush(ColorBlue).Get());
            }
        }

        XMFLOAT2 current_left;
        XMFLOAT2 current_right = SplineInterpolate(0.0f);
        double alpha = 1 / 60.0f;
        for (double t = alpha; t < 1.0f; t += alpha)
        {
            current_left = current_right;
            // Calculate the point on the spline at t
            current_right = SplineInterpolate(t);
            // Convert to screen coordinates for D2D
            XMFLOAT2 points[2];
            points[0] = ConvertToScreenCoordinates(XMFLOAT3(current_left.x, m_GroundLevel, current_left.y), m_World);
            points[1] = ConvertToScreenCoordinates(XMFLOAT3(current_right.x, m_GroundLevel, current_right.y), m_World);
            // Draw line from current point to next point
            m_spD2DDeviceContext->DrawLine(
                D2D1::Point2F(points[0].x, points[0].y),
                D2D1::Point2F(points[1].x, points[1].y),
                GetD2DBrush(ColorOrange).Get());
        }

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    void GraphicsSystem::DrawIKTarget(){
        m_spD2DDeviceContext->BeginDraw();

        // Draw current target point
        // Convert from world to screen coordinates
        XMFLOAT2 point;
        point = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(m_IKTargetPosition, m_World);

        m_spD2DDeviceContext->DrawEllipse(
            D2D1::Ellipse(
            D2D1::Point2F(point.x, point.y),
            m_PathPointRadius, m_PathPointRadius),
            GetD2DBrush(ColorRed).Get());

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    float LineSegmentLength(XMFLOAT2 p0, XMFLOAT2 p1){
        return sqrt(pow((p1.x - p0.x), 2.0f) + pow((p1.y - p0.y), 2.0f));
    }

    void GraphicsSystem::BuildArcLengthTable()
    {
        // Initially use the same number of arc table entries as control points
        m_ArcLengthTable.resize(m_PathControlPoints.size());

        // Initialize the first point to 0, 0
        std::list<ArcLengthTableElement>::iterator it1 = m_ArcLengthTable.begin();
        it1->u = 0.0f;
        it1->s = 0.0f;
        it1->p = SplineInterpolate(it1->u);

        std::list<ArcLengthTableElement>::iterator it2 = it1;
        ++it2;

        // Build the initial arc table
        for (size_t i = 1; i < m_ArcLengthTable.size(); ++i)
        {
            it2->u = (double)i / (double)(m_ArcLengthTable.size() - 1);
            it2->p = SplineInterpolate(it2->u);
            it2->s = LineSegmentLength(it1->p, it2->p) + it1->s;
            ++it1;
            ++it2;
        }

        it1 = it2 = m_ArcLengthTable.begin();
        ++it2;
        while (it2 != m_ArcLengthTable.end()){
            // Determine if the points in the arc length table at it1 and it2 accurately measure the length
            // based on the midpoint between the two
            ArcLengthTableElement m;
            m.u = (it1->u + it2->u) / 2.0f;
            m.p = SplineInterpolate(m.u);

            double A = LineSegmentLength(it1->p, m.p);
            double B = LineSegmentLength(m.p, it2->p);
            double C = LineSegmentLength(it1->p, it2->p);
            double d = A + B - C;

            // Insert the midpoint
            m.s = A + it1->s;
            it2 = m_ArcLengthTable.insert(it2, m);

            // Update the remaining points with new lengths
            std::list<ArcLengthTableElement>::iterator updateIt = it2;
            ++updateIt;
            double diff = A + B + it1->s - updateIt->s;
            for (; updateIt != m_ArcLengthTable.end(); ++updateIt)
            {
                updateIt->s += diff;
            }

            // If the midpoint between it1 and it2 is close to the line segment between it1 and it2
            if (d < m_ArcLengthEpsilon){
                // Move on
                ++it1;
                ++it1;
                ++it2;
                ++it2;
            }
            // Otherwise continue subdividing between it1 and the midpoint
        }
        
        m_ArcLength = m_ArcLengthTable.back().s;
        m_AnimationLength = (float)m_ArcLength / 7.0f;
        m_NormalizedDistanceAlongArc = 0.0f;
    }

    void GraphicsSystem::NormalizeArcLengthTable(){
        for (std::list<ArcLengthTableElement>::iterator it = m_ArcLengthTable.begin(); it != m_ArcLengthTable.end(); ++it)
        {
            it->s /= m_ArcLength;
        }
    }

    std::list<ArcLengthTableElement>::iterator GraphicsSystem::BinaryArcLookupByLength(double len, size_t begin, size_t end){
        size_t midpoint = begin + (end - begin) / 2;
        std::list<ArcLengthTableElement>::iterator it = m_ArcLengthTable.begin();
        for (size_t i = 0; i < midpoint; ++i)
        {
            ++it;
        }
        // if we have reached the end of the search
        if (end - begin <= 1)
        {
            return it;
        }
        // if len is in the first half of the list
        else if (it->s > len)
        {
            return BinaryArcLookupByLength(len, begin, midpoint);
        }
        // if len is in the second half of the list
        else if (it->s < len)
        {
            return BinaryArcLookupByLength(len, midpoint, end);
        }
        else
        {
            return it;
        }
    }

    std::list<ArcLengthTableElement>::iterator GraphicsSystem::BinaryArcLookupByTime(double t, size_t begin, size_t end){
        size_t midpoint = begin + (end - begin) / 2;
        std::list<ArcLengthTableElement>::iterator it = m_ArcLengthTable.begin();
        for (size_t i = 0; i < midpoint; ++i)
        {
            ++it;
        }
        // if we have reached the end of the search
        if (end - begin <= 1)
        {
            return it;
        }
        // if t is in the first half of the list
        else if (it->u > t)
        {
            return BinaryArcLookupByTime(t, begin, midpoint);
        }
        // if t is in the second half of the list
        else if (it->u < t)
        {
            return BinaryArcLookupByTime(t, midpoint, end);
        }
        else
        {
            return it;
        }
    }

    double InterpolateByTime(std::list<ArcLengthTableElement>::iterator it1, std::list<ArcLengthTableElement>::iterator it2, double t)
    {
        double k = (t - it1->u) / (it2->u - it1->u);
        return it1->s + k * (it2->s - it1->s);
    }

    double InterpolateByLength(std::list<ArcLengthTableElement>::iterator it1, std::list<ArcLengthTableElement>::iterator it2, double len)
    {
        double k = (len - it1->s) / (it2->s - it1->s);
        return it1->u + k * (it2->u - it1->u);
    }

    // Returns the time t at length len
    double GraphicsSystem::ArcLength(double t)
    {
        ThrowErrorIf(t > 1.f + FLT_EPSILON, "Searching for a time larger than the (normalized) arc itself. Be sure to normalize len before passing it in.");

        // Perform a binary search on the arc length table
        std::list<ArcLengthTableElement>::iterator it1 = BinaryArcLookupByTime(t, 0, m_ArcLengthTable.size() - 1);
        std::list<ArcLengthTableElement>::iterator it2 = it1;

        // If it is not an exact match
        if (it1->u < t - FLT_EPSILON)
        {
            // The iterator we have is to the element before t.
            // Now get the element after t.
            ++it2;
            if (it2 == m_ArcLengthTable.end()){ return it1->s; }
            return InterpolateByTime(it1, it2, t);
        }
        else if (it1->u > t + FLT_EPSILON)
        {
            // The iterator we have is to the element after t.
            // Now get the element before t.
            if (it1 == m_ArcLengthTable.begin()){ return it1->s; }
            --it2;
            return InterpolateByTime(it2, it1, t);
        }
        else
        {
            // We already have an element at exactly len.
            return it1->u;
        }
    }

    // Returns the time t at length len
    double GraphicsSystem::InverseArcLength(double len)
    {
        ThrowErrorIf(len > 1.f + FLT_EPSILON, "Searching for a length larger than the (normalized) arc itself. Be sure to normalize len before passing it in.");

        // Perform a binary search on the arc length table
        std::list<ArcLengthTableElement>::iterator it1 = BinaryArcLookupByLength(len, 0, m_ArcLengthTable.size() - 1);
        std::list<ArcLengthTableElement>::iterator it2 = it1;
        
        // If it is not an exact match
        if (it1->s < len - FLT_EPSILON)
        {
            // The iterator we have is to the element before t.
            // Now get the element after t.
            ++it2;
            if (it2 == m_ArcLengthTable.end()){ return it1->s; }
            return InterpolateByLength(it1, it2, len);
        }
        else if (it1->s > len + FLT_EPSILON)
        {
            // The iterator we have is to the element after t.
            // Now get the element before t.
            if (it1 == m_ArcLengthTable.begin()){ return it1->s; }
            --it2;
            return InterpolateByLength(it2, it1, len);
        }
        else
        {
            // We already have an element at exactly len.
            return it1->s;
        }
    }

    XMFLOAT3 Midpoint(XMFLOAT3 p0, XMFLOAT3 p1)
    {
        XMFLOAT3 midpoint(0.0f, 0.0f, 0.0f);
        midpoint.x = (p0.x + p1.x) / 2.0f;
        midpoint.y = (p0.y + p1.y) / 2.0f;
        midpoint.z = (p0.z + p1.z) / 2.0f;
        return midpoint;
    }

    std::vector<std::vector<XMFLOAT3>> BuildMidpointTableFromPoints(std::vector<XMFLOAT3> points)
    {
        std::vector<std::vector<XMFLOAT3>> Table;
        // Row 0
        Table.push_back(points);

        for (int i = 1; i < Table[0].size(); ++i)
        {
            std::vector<XMFLOAT3> RowI;
            Table.push_back(RowI);
            // For each row, iterate through the previous row and generate a new row of midpoints
            for (int j = 0; j < Table[i - 1].size() - 1; ++j)
            {
                Table[i].push_back(Midpoint(Table[i - 1][j], Table[i - 1][j + 1]));
            }
        }

        return Table;
    }

    std::vector<XMFLOAT3> GraphicsSystem::ExpandMidpointList(std::vector<XMFLOAT3> Midpoints)
    {
        std::vector<XMFLOAT3> ExpandedList;
        // Add the first point to the expanded list
        ExpandedList.push_back(Midpoints.front());
        // Generate the table of midpoints
        for (size_t i = 0; i < Midpoints.size() - (m_PathControlPoints.size() - 1); i += (m_PathControlPoints.size() - 1))
        {
            std::vector<XMFLOAT3> points;
            for (size_t j = i; j < i + m_PathControlPoints.size(); ++j)
            {
                points.push_back(Midpoints[j]);
            }

            std::vector<std::vector<XMFLOAT3>> Table = BuildMidpointTableFromPoints(points);

            // Add the top diagonal of the table to the expanded list
            for (size_t j = 1; j < Table.size(); ++j)
            {
                ExpandedList.push_back(Table[j][0]);
            }
            // Add the bottom diagonal of the table to the expanded list
            for (size_t j = 2; j <= Table.size(); ++j)
            {
                ExpandedList.push_back(Table[Table.size() - j][j - 1]);
            }
        }

        return ExpandedList;
    }
    
    void GraphicsSystem::DrawPathMidpoint(){
        m_spD2DDeviceContext->BeginDraw();
        // Draw each path point
        std::vector<XMFLOAT3>::iterator it1 = m_PathControlPoints.begin();

        // Generate the MidPoint List
        std::vector<XMFLOAT3> MidPointPoints = m_PathControlPoints; // k = 1
        int NumApproximations = 6;
        for (int k = 1; k < NumApproximations; ++k)
        {
            std::vector<XMFLOAT3> NextApproximation = ExpandMidpointList(MidPointPoints);
            MidPointPoints = NextApproximation;
        }

        for (; it1 != m_PathControlPoints.end(); ++it1)
        {

            // Draw current path point
            // Convert from world to screen coordinates
            XMFLOAT2 points[2];
            points[0] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(*it1, m_World);

            m_spD2DDeviceContext->DrawEllipse(
                D2D1::Ellipse(
                D2D1::Point2F(points[0].x, points[0].y),
                m_PathPointRadius, m_PathPointRadius),
                GetD2DBrush(ColorRed).Get());

            // Draw line from current point to next point
            std::vector<Vector3>::iterator it2 = it1;
            ++it2;
            if (it2 != m_PathControlPoints.end())
            {
                points[1] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(*it2, m_World);

                // Use D2D to draw a line in screen coordinates
                m_spD2DDeviceContext->DrawLine(
                    D2D1::Point2F(points[0].x, points[0].y),
                    D2D1::Point2F(points[1].x, points[1].y),
                    GetD2DBrush(ColorRed).Get());
            }
        }

        for (size_t i = 0; i < MidPointPoints.size() - 1; ++i)
        {
            // Convert from world to screen coordinates
            XMFLOAT2 points[2];
            points[1] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(MidPointPoints[i + 1], m_World);
            points[0] = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(MidPointPoints[i], m_World);

            // Draw line from current point to next point
            m_spD2DDeviceContext->DrawLine(
                D2D1::Point2F(points[0].x, points[0].y),
                D2D1::Point2F(points[1].x, points[1].y),
                GetD2DBrush(ColorOrange).Get());
        }
        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    void GraphicsSystem::DrawMouse(){
        m_spD2DDeviceContext->BeginDraw();
        m_spD2DDeviceContext->DrawEllipse(
            D2D1::Ellipse(
            D2D1::Point2F(m_MousePos.x, m_MousePos.y),
            2.0f, 2.0f),
            GetD2DBrush(ColorBlue).Get(),
            2.0f);
        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

	void GraphicsSystem::DrawSelected(){        
        m_spD2DDeviceContext->BeginDraw();

        // Draw debug info to show a model is the current object.
        std::list<Model*>::iterator itM = m_ModelList.begin();
        for(;itM!=m_ModelList.end();++itM)
        {
			if((*itM)->IsSelected())
				DrawDebug(*itM);
        }

        // Draw debug info to show a sprite is the current object.
        std::list<Sprite*>::iterator itBS = m_BackgroundSpriteList.begin();
        for(;itBS!=m_BackgroundSpriteList.end();++itBS)
        {
			if((*itBS)->IsSelected())
				DrawDebug(*itBS);
        }
        std::list<Sprite*>::iterator itS = m_SpriteList.begin();
        for(;itS!=m_SpriteList.end();++itS)
        {
			if((*itS)->IsSelected())
				DrawDebug(*itS);
        }

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    bool SpriteDepthCompare(Sprite* first, Sprite* second)
    {
        return first->GetWorldTransform().m[3][2] > second->GetWorldTransform().m[3][2];
    }

    bool TextDepthCompare(Text* first, Text* second)
    {
        return first->GetPosition().z < second->GetPosition().z;
    }

    bool ImageDepthCompare(Image* first, Image* second)
    {
        return first->GetPosition().z < second->GetPosition().z;
    }

    void GraphicsSystem::UpdateBackgroundSprites(float dt){
        // Update each object
        std::list<Sprite*>::iterator it = m_BackgroundSpriteList.begin();
        for(;it!=m_BackgroundSpriteList.end();++it)
        {
            (*it)->Update(dt);
        }
        SortBackgroundSprites();
    }

    void GraphicsSystem::UpdateSprites(float dt){
        // Update each object
        std::list<Sprite*>::iterator it = m_SpriteList.begin();
        for(;it!=m_SpriteList.end();++it)
        {
            (*it)->Update(dt);
        }
        SortSprites();
    }

    void GraphicsSystem::UpdateImages(float dt){
        // Draw each object
        std::list<Image*>::iterator it = m_ImageList.begin();
        for(;it!=m_ImageList.end();++it)
        {
            (*it)->Update(dt);
        }
        SortImages();
    }

    void GraphicsSystem::SortText(){
        m_TextList.sort(TextDepthCompare);
    }

    void GraphicsSystem::SortImages(){
        m_ImageList.sort(ImageDepthCompare);
    }

    void GraphicsSystem::SortSprites(){      
        if(m_SpriteList.size() > 1)
        {
            m_SpriteList.sort(SpriteDepthCompare);
        }
    }
    void GraphicsSystem::SortBackgroundSprites(){      
        if(m_BackgroundSpriteList.size() > 1)
        {
            m_BackgroundSpriteList.sort(SpriteDepthCompare);
        }
    }

    void GraphicsSystem::DrawSprites(){
        // Draw each object
        std::list<Sprite*>::iterator it = m_SpriteList.begin();
        for(;it!=m_SpriteList.end();++it)
        {
            (*it)->Draw(m_spD3DDeviceContext1, m_spConstantBufferPerSpriteObject);
        }
    }
    void GraphicsSystem::DrawBackgroundSprites(){
        // Draw each object
        std::list<Sprite*>::iterator it = m_BackgroundSpriteList.begin();
        for(;it!=m_BackgroundSpriteList.end();++it)
        {
            (*it)->Draw(m_spD3DDeviceContext1, m_spConstantBufferPerSpriteObject);
        }
    }

    void GraphicsSystem::DrawTextComponents(){
        // Draw each object
        std::list<Text*>::iterator it = m_TextList.begin();
        for(;it!=m_TextList.end();++it)
        {
            (*it)->Draw(m_spD2DDeviceContext, m_spWhiteBrush, m_spDWriteFactory);
        }
    }

    void GraphicsSystem::DrawImages(){
        // Draw each object
        std::list<Image*>::iterator it = m_ImageList.begin();
        for(;it!=m_ImageList.end();++it)
        {
            (*it)->Draw(m_spD2DDeviceContext);
        }
    }

    void GraphicsSystem::DrawDebugInfo(){        
        m_spD2DDeviceContext->BeginDraw();

        // Draw debug info for each model
        std::list<Model*>::iterator itM = m_ModelList.begin();
        for(;itM!=m_ModelList.end();++itM)
        {
            DrawDebug(*itM);
        }

        // Draw debug info for each sprite
        std::list<Sprite*>::iterator itS = m_SpriteList.begin();
        for(;itS!=m_SpriteList.end();++itS)
        {
            DrawDebug(*itS);
        }

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    
    void GraphicsSystem::DrawDebug(Model* pModel){
        RigidBody* pPhysics = pModel->GetOwner()->has(RigidBody);

        // Draw a 2D projected square at the center (in the z direction) of the model
        DrawProjectedDebugBox(
            pModel->GetWorldTransform(),
            pPhysics
            );

        
        XMFLOAT4X4 proj;
        for(float i = TRACK_DEPTH; i < pModel->GetSize().z * 0.5f; i += TRACK_DEPTH){
            // Translate back the depth of one track and draw a debug rect
            XMStoreFloat4x4(&proj, XMLoadFloat4x4(&pModel->GetWorldTransform()) * XMMatrixTranslation(0.0f, 0.0f, i));

            DrawProjectedDebugBox(
                proj,
                pPhysics
            );

            // Translate forward the depth of one track and draw a debug rect
            XMStoreFloat4x4(&proj, XMLoadFloat4x4(&pModel->GetWorldTransform()) * XMMatrixTranslation(0.0f, 0.0f, -i));

            DrawProjectedDebugBox(
                proj,
                pPhysics
            );
        }
    }

    void GraphicsSystem::DrawDebug(Sprite* pSprite){
        RigidBody* pPhysics = pSprite->GetOwner()->has(RigidBody);
        DrawProjectedDebugBox(
            pSprite->GetWorldTransform(),
            pPhysics
            );
    }

    void GraphicsSystem::DrawProjectedDebugBox(XMFLOAT4X4 worldTransform, RigidBody* pPhysics){
        
        XMFLOAT2 screenCoordinateTopLeft = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(XMFLOAT3(0.0f - 0.5f, 0.0f + 0.5f, 0.0f), worldTransform);
        XMFLOAT2 screenCoordinateBottomRight = g_GRAPHICSSYSTEM->ConvertToScreenCoordinates(XMFLOAT3(0.0f + 0.5f, 0.0f - 0.5f, 0.0f), worldTransform);

        // Choose the brush color based on the z-depth (worldTransform.m[3][2]) of the object
        D2D1_COLOR_F BrushColor;
        if(!pPhysics)
            BrushColor = D2D1::ColorF(D2D1::ColorF::White);
        else if(worldTransform.m[3][2] > 0.5f)
            BrushColor = D2D1::ColorF(D2D1::ColorF::Red);
        else if(worldTransform.m[3][2] < -0.5f)
            BrushColor = D2D1::ColorF(D2D1::ColorF::Yellow);
        else
            BrushColor = D2D1::ColorF(D2D1::ColorF::Orange);

        ComPtr<ID2D1SolidColorBrush> spSolidBrush;
        m_spD2DDeviceContext->CreateSolidColorBrush(
            BrushColor,
            &spSolidBrush);

        // Draw the Rect
        m_spD2DDeviceContext->DrawRectangle(
            D2D1::RectF(
                screenCoordinateTopLeft.x,
                screenCoordinateTopLeft.y,
                screenCoordinateBottomRight.x,
                screenCoordinateBottomRight.y),
                spSolidBrush.Get());

        // If the sprite has a physics component, create and draw a line to indicate velocity
        if(pPhysics){
            // Create Geometry for the velocity
            ComPtr<ID2D1PathGeometry> spVelocityGeometry;
            DXThrowIfFailed(m_spD2DFactory->CreatePathGeometry(&spVelocityGeometry));
            ComPtr<ID2D1GeometrySink> spGeometrySink;
            DXThrowIfFailed(spVelocityGeometry->Open(&spGeometrySink));

            spGeometrySink->BeginFigure(
                D2D1::Point2F(
                    (screenCoordinateTopLeft.x + screenCoordinateBottomRight.x) * 0.5f,
                    (screenCoordinateTopLeft.y + screenCoordinateBottomRight.y) * 0.5f),
                    D2D1_FIGURE_BEGIN_FILLED);

            spGeometrySink->AddLine(
                D2D1::Point2F(
                pPhysics->velocity.x * 3.0f + (screenCoordinateTopLeft.x + screenCoordinateBottomRight.x) * 0.5f,
                (screenCoordinateTopLeft.y + screenCoordinateBottomRight.y) * 0.5f - pPhysics->velocity.y * 3.0f));

            spGeometrySink->EndFigure(D2D1_FIGURE_END_OPEN);

            DXThrowIfFailed(spGeometrySink->Close());

            // Draw the velocity geometry
            m_spD2DDeviceContext->DrawGeometry(spVelocityGeometry.Get(), spSolidBrush.Get());
        }
    }

	void GraphicsSystem::RemoveModelComponent(int id){
		std::list<Model*>::iterator it = m_ModelList.begin();
        for(;it!=m_ModelList.end();++it)
        {
			if((*it)->GetOwnerID() == id)
			{
				m_ModelList.erase(it);
				return;	
			}
        }
	}

    void GraphicsSystem::RemoveSpriteComponent(int id){
		std::list<Sprite*>::iterator it = m_SpriteList.begin();
        for(;it!=m_SpriteList.end();++it)
        {
			if((*it)->GetOwnerID() == id)
			{
				m_SpriteList.erase(it);
				return;	
			}
        }
        std::list<Sprite*>::iterator itB = m_BackgroundSpriteList.begin();
        for(;itB!=m_BackgroundSpriteList.end();++itB)
        {
			if((*itB)->GetOwnerID() == id)
			{
				m_BackgroundSpriteList.erase(it);
				return;	
			}
        }
	}

    void GraphicsSystem::RemoveCameraComponent(int id){
		std::list<Camera*>::iterator it = m_CameraList.begin();
        for(;it!=m_CameraList.end();++it)
        {
			if((*it)->GetOwnerID() == id)
			{
				m_CameraList.erase(it);
				return;	
			}
        }
	}

    void GraphicsSystem::RemoveTextComponent(int id){
		std::list<Text*>::iterator it = m_TextList.begin();
        for(;it!=m_TextList.end();++it)
        {
			if((*it)->GetOwnerID() == id)
			{
				m_TextList.erase(it);
				return;	
			}
        }
	}

    void GraphicsSystem::RemoveImageComponent(int id){
		std::list<Image*>::iterator it = m_ImageList.begin();
        for(;it!=m_ImageList.end();++it)
        {
			if((*it)->GetOwnerID() == id)
			{
				m_ImageList.erase(it);
				return;	
			}
        }
	}

    void GraphicsSystem::LoadResources(){
        // Create Factory, Device, and Window Resources
        CreateDeviceIndependentResources();
        CreateDeviceResources();
        CreateWindowSizeDependentResources();

        /*Attempting to get rid of a depth clip
        D3D11_RASTERIZER_DESC1 rd;
        rd.CullMode = D3D11_CULL_NONE;
        rd.DepthClipEnable = false;
        

        ComPtr<ID3D11RasterizerState1> spRS;
        m_spD3DDevice1->CreateRasterizerState1(&rd, spRS.GetAddressOf());
        m_spD3DDeviceContext1->RSSetState(spRS.Get());*/

        // Create 3D Resources

		// Setup the viewport
		/*D3D11_VIEWPORT vp;
        vp.Width = (FLOAT)m_WindowSize.x;
        vp.Height = (FLOAT)m_WindowSize.y;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		m_spD3DDeviceContext1->RSSetViewports(1, &vp);*/

        // Define the input layout
        D3D11_INPUT_ELEMENT_DESC FBXBinModelLayout[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        CreateShaders(L"Shaders\\GBufferTexturedShader.fx", FBXBinModelLayout, ARRAYSIZE(FBXBinModelLayout), m_spFBXBinModelVertexLayout, "VS", m_spGBufferFBXBinModelVertexShader, "PS", m_spGBufferFBXBinModelPixelShader);

        /*
        // Define the input layout
		D3D11_INPUT_ELEMENT_DESC AlbedoModelLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

        CreateShaders(L"Shaders\\GBufferShader.fx", AlbedoModelLayout, ARRAYSIZE(AlbedoModelLayout), m_spAlbedoModelVertexLayout, m_spGBufferAlbedoModelVertexShader, m_spGBufferAlbedoModelPixelShader);

        // Define the input layout
		D3D11_INPUT_ELEMENT_DESC TexturedModelLayout[] =
		{
			{ "POSITION",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD",  0, DXGI_FORMAT_R32G32_FLOAT,    0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

        CreateShaders(L"Shaders\\GBufferTexturedShader.fx", TexturedModelLayout, ARRAYSIZE(TexturedModelLayout), m_spTexturedModelVertexLayout, m_spGBufferTexturedModelVertexShader, m_spGBufferTexturedModelPixelShader);
        */
        // Define the sprite input layout
        D3D11_INPUT_ELEMENT_DESC SpriteLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 28, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
        CreateShaders(L"Shaders\\SpriteShader.fx", SpriteLayout, ARRAYSIZE(SpriteLayout), m_spSpriteVertexLayout, "VS", m_spSpriteVertexShader, "PS", m_spSpritePixelShader);
        
        CreateShaders(L"Shaders\\GBufferVisualizer.fx", SpriteLayout, ARRAYSIZE(SpriteLayout), m_spLightVertexLayout, "VS", m_spGBufferVisualizerVertexShader, "PS", m_spGBufferVisualizerPixelShader);
        
        D3D11_INPUT_ELEMENT_DESC FullScreenQuadLayout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT,    0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
        CreateShaders(L"Shaders\\DirectionalLight.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "VS", m_spDirectionalLightVertexShader, "PS", m_spDirectionalLightPixelShader);
        
        CreateShaders(L"Shaders\\ContourDetectionPass1.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "VS", m_spContourDetectionPass1VertexShader, "PS", m_spContourDetectionPass1PixelShader);
        CreateShaders(L"Shaders\\ContourDetectionPass2.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "VS", m_spContourDetectionPass2VertexShader, "PS", m_spContourDetectionPass2PixelShader);
        CreateShaders(L"Shaders\\LuminanceVisualizer.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "VS", m_spLuminanceVertexShader, "PS", m_spLuminancePixelShader);
        CreateShaders(L"Shaders\\DownSample.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "DownSample_VS", m_spDownSampleVertexShader, "DownSampleQuarterRes_PS", m_spQuarterResDownSamplePixelShader);
        CreatePixelShader(L"Shaders\\DownSample.fx", "UpSampleQuarterRes_PS", m_spQuarterResUpSamplePixelShader);
        CreatePixelShader(L"Shaders\\GradientBuffer.fx", "HorizontalSobel_PS", m_spHorizontalSobelPixelShader);
        CreatePixelShader(L"Shaders\\GradientBuffer.fx", "VerticalSobel_PS", m_spVerticalSobelPixelShader);
        //CreateShaders(L"Shaders\\DownSample.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "UpSample_VS", m_spUpSampleVertexShader, "UpSample_PS", m_spUpSamplePixelShader);
        CreatePixelShader(L"Shaders\\GradientBuffer.fx", "GaussianBlurHorizontal_PS", m_spHorizontalGaussianBlurPixelShader);
        CreatePixelShader(L"Shaders\\GradientBuffer.fx", "GaussianBlurVertical_PS", m_spVerticalGaussianBlurPixelShader);
        CreateShaders(L"Shaders\\BufferVisualizer.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "VS", m_spBufferVisualizerVertexShader, "PS", m_spBufferVisualizerPixelShader);
        CreatePixelShader(L"Shaders\\BufferVisualizer.fx", "PS_Red", m_spBufferVisualizerRedPixelShader);
        CreatePixelShader(L"Shaders\\BufferVisualizer.fx", "PS_Green", m_spBufferVisualizerGreenPixelShader);
        CreatePixelShader(L"Shaders\\BufferVisualizer.fx", "PS_Blue", m_spBufferVisualizerBluePixelShader);
        CreateShaders(L"Shaders\\LuminanceStrokeDirection.fx", FBXBinModelLayout, ARRAYSIZE(FBXBinModelLayout), m_spFBXBinModelVertexLayout, "LuminanceStrokeDirection_VS", m_spPerPixelToPerVertexStrokeDirectionVertexShader, "LuminanceStrokeDirection_PS", m_spPerPixelToPerVertexStrokeDirectionPixelShader);
        CreateShaders(L"Shaders\\PrincipalCurvatureStrokeDirection.fx", FullScreenQuadLayout, ARRAYSIZE(FullScreenQuadLayout), m_spFullScreenQuadVertexLayout, "VS", m_spPrincipalCurvatureStrokeDirectionVertexShader, "PS", m_spPrincipalCurvatureStrokeDirectionPixelShader);
        CreatePixelShader(L"Shaders\\RenderStrokes.fx", "RenderStrokes_PS", m_spRenderStrokesPixelShader);

        CreateBuffers();
        CreateQuadResources();
        InitializeMatrices();
        LoadModels();

        // Create 2D Resources       
        CreateBrushes();
        CreateSamplers();
        LoadTextures();
    }

    void GraphicsSystem::CreateDeviceIndependentResources(){
        D2D1_FACTORY_OPTIONS options;
        ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
        // If the project is in a debug build, enable Direct2D debugging via SDK Layers.
        options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
        DXThrowIfFailed(
			D2D1CreateFactory(
			D2D1_FACTORY_TYPE_SINGLE_THREADED,
				__uuidof(ID2D1Factory1),
				&options,
				&m_spD2DFactory
				)
			);

        DXThrowIfFailed(
            DWriteCreateFactory(
                DWRITE_FACTORY_TYPE_SHARED,
                __uuidof(IDWriteFactory1),
                &m_spDWriteFactory
                )
            );
        
        DXThrowIfFailed(
            CoCreateInstance(
                CLSID_WICImagingFactory,
                nullptr,
                CLSCTX_INPROC_SERVER,
                IID_PPV_ARGS(&m_spWICFactory)
                )
            );
    }

    void GraphicsSystem::CreateDeviceResources(){
        // BGRA_SUPPORT flag adds support for surfaces with a different color channel ordering
        // than the API default. It is required for compatibility with Direct2D.
        UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

        // SINGLETHREADED flag improves performance. It will need to change if we make multithreaded graphics calls
        creationFlags |= D3D11_CREATE_DEVICE_SINGLETHREADED;
 
#if defined(_DEBUG)
    // If the project is in a debug build, enable debugging via SDK Layers with this flag.
    creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

        // This array defines the set of DirectX hardware feature levels this app will support.
        // Note the ordering should be preserved.
        // Don't forget to declare your application's minimum required feature level in its
        // description.  All applications are assumed to support 9.1 unless otherwise stated.
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0
        };

        // Create the Direct3D 11 API device object and a corresponding context.
        ComPtr<ID3D11Device> spDevice;
        ComPtr<ID3D11DeviceContext> spContext;
        HRESULT hr = D3D11CreateDevice(
                nullptr,                    // Specify nullptr to use the default adapter.
                D3D_DRIVER_TYPE_HARDWARE,
                0,
                creationFlags,              // Set debug and Direct2D compatibility flags.
                featureLevels,              // List of feature levels this app can support.
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
                &spDevice,                  // Returns the Direct3D device created.
                &m_FeatureLevel,            // Returns feature level of device created.
                &spContext                  // Returns the device immediate context.
            );

        // Fall back to software if hardware creation fails
		if(DXGI_ERROR_UNSUPPORTED == hr)
		{
            hr = D3D11CreateDevice(
                nullptr,                    // Specify nullptr to use the default adapter.
                D3D_DRIVER_TYPE_WARP,
                0,
                creationFlags,              // Set debug and Direct2D compatibility flags.
                featureLevels,              // List of feature levels this app can support.
                ARRAYSIZE(featureLevels),
                D3D11_SDK_VERSION,          // Always set this to D3D11_SDK_VERSION for Windows Store apps.
                &spDevice,                  // Returns the Direct3D device created.
                &m_FeatureLevel,            // Returns feature level of device created.
                &spContext                  // Returns the device immediate context.
                );
		}
        DXThrowIfFailed(hr);

        // Get the Direct3D 11.1 API device and context interfaces.
        DXThrowIfFailed(
            spDevice.As(&m_spD3DDevice1)
            );

        DXThrowIfFailed(
            spContext.As(&m_spD3DDeviceContext1)
            );

        // Get the underlying DXGI device of the Direct3D device.
		ComPtr<IDXGIDevice2> spDXGIDevice;
        DXThrowIfFailed(
            m_spD3DDevice1.As(&spDXGIDevice)
            );

        // Create the Direct2D device object and a corresponding context.
        DXThrowIfFailed(
            m_spD2DFactory->CreateDevice(spDXGIDevice.Get(), &m_spD2DDevice)
            );

        DXThrowIfFailed(
            m_spD2DDevice->CreateDeviceContext(
                D2D1_DEVICE_CONTEXT_OPTIONS_NONE,
                &m_spD2DDeviceContext
                )
            );
    }

    // Allocate all memory resources that change on a window SizeChanged event.
    void GraphicsSystem::CreateWindowSizeDependentResources(){
        RECT rc;
        GetClientRect(m_Window->m_hwnd, &rc);
		m_WindowSize.x = rc.right - rc.left;
		m_WindowSize.y = rc.bottom - rc.top;

        // If the swap chain already exists, resize it.
        if (m_spSwapChain1 != nullptr)
        {
            // Existing swap chain needs to be resized.
            // Make sure that the dependent objects have been released.
            m_spD3DDeviceContext1->OMSetRenderTargets(0, nullptr, nullptr);
            m_spD3DRenderTargetView = nullptr;
            m_spIntermediateRTV = nullptr;
            m_spIntermediateRTVDebug = nullptr;
            //m_GBuffer.GetDepthStencilView() = nullptr;
            m_spD2DDeviceContext->SetTarget(nullptr);
            m_spD2DTargetBitmap = nullptr;

            DXThrowIfFailed(
                m_spSwapChain1->ResizeBuffers(
                    2,
                    static_cast<UINT>(m_WindowSize.x),
                    static_cast<UINT>(m_WindowSize.y),
                    DXGI_FORMAT_B8G8R8A8_UNORM,
                    0
                    ));
        }
        else
        {
            // Otherwise, create a new swapchain using the same adapter as the existing Direct3D device.
            DXGI_SWAP_CHAIN_DESC1 swapChainDesc;

            ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC1));

            swapChainDesc.Width = m_WindowSize.x;                            // Use automatic sizing.
            swapChainDesc.Height = m_WindowSize.y;
            swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;              // This is the most common swap chain format.
            swapChainDesc.Stereo = false;
            swapChainDesc.SampleDesc.Count = 1;                             // Use 4x MSAA (all D3D 11 devices support 4x MSAA)
            swapChainDesc.SampleDesc.Quality = 0;
            swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;    // Use RENDER_TARGET_OUTPUT because we are going to be rendering to the back buffer

            // When you call IDXGIFactory::CreateSwapChain to create a full-screen swap chain, you typically include the front buffer in this value.
            swapChainDesc.BufferCount = 1;                           // Use one back buffer for double-buffering to minimize latency.
            swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
            swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;    // Let the display driver select the most efficient presentation method.
            swapChainDesc.Flags = 0;
            swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;


		    ComPtr<IDXGIDevice2> spDXGIDevice;
            DXThrowIfFailed(
                m_spD3DDevice1.As(&spDXGIDevice)
                );

            ComPtr<IDXGIAdapter> spDXGIAdapter;
            DXThrowIfFailed(
                spDXGIDevice->GetAdapter(&spDXGIAdapter)
                );

            ComPtr<IDXGIFactory2> spDXGIFactory;
            DXThrowIfFailed(
                spDXGIAdapter->GetParent(IID_PPV_ARGS(&spDXGIFactory))
                );

            DXThrowIfFailed(
                spDXGIFactory->CreateSwapChainForHwnd(
                    m_spD3DDevice1.Get(),
                    m_Window->m_hwnd,
                    &swapChainDesc,
                    nullptr,
                    nullptr,
                    &m_spSwapChain1
                    )
                );

            // Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
            // ensures that the application will only render after each VSync, minimizing power consumption.
            DXThrowIfFailed(
                spDXGIDevice->SetMaximumFrameLatency(1)
                );
        }

        // Create a Direct3D render target view of the swap chain back buffer.
        ComPtr<ID3D11Texture2D> spBackBuffer;
        DXThrowIfFailed(
            m_spSwapChain1->GetBuffer(0, IID_PPV_ARGS(&spBackBuffer))
            );

        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                spBackBuffer.Get(),
                nullptr,
                &m_spD3DRenderTargetView
                )
            );        

        // Initialize the GBuffer resources
        D3D11_TEXTURE2D_DESC backBufferDesc;
        spBackBuffer->GetDesc(&backBufferDesc);

        D3D11_TEXTURE2D_DESC depthStencilDesc = {
        backBufferDesc.Width, //UINT Width;
        backBufferDesc.Height, //UINT Height;
		1, //UINT MipLevels;
		1, //UINT ArraySize;
		DXGI_FORMAT_UNKNOWN, //DXGI_FORMAT Format;
		1, //DXGI_SAMPLE_DESC SampleDesc;
		0,
		D3D11_USAGE_DEFAULT,//D3D11_USAGE Usage;
		D3D11_BIND_DEPTH_STENCIL|D3D11_BIND_SHADER_RESOURCE,//UINT BindFlags;
		0,//UINT CPUAccessFlags;
		0//UINT MiscFlags;    
        };

        m_GBuffer.Initialize(m_spD3DDevice1, depthStencilDesc);

        // Create an intermediate render target that can be used to render to a texture
        backBufferDesc.Format = m_IntermediateRTFormat;
        backBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &backBufferDesc,
                nullptr,
                &m_spIntermediateRT
                )
            );

        // Create the color and specular intensity render target view
        CD3D11_RENDER_TARGET_VIEW_DESC RTVDesc(
            D3D11_RTV_DIMENSION_TEXTURE2D,
            m_IntermediateRTFormat
            );

        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spIntermediateRT.Get(),
                &RTVDesc,
                &m_spIntermediateRTV
                )
            );
        
        // Create the color and specular intensity shader resource view
        CD3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc(
            D3D11_SRV_DIMENSION_TEXTURE2D,
            m_IntermediateRTFormat
            );

        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spIntermediateRT.Get(),
                &SRVDesc,
                &m_spIntermediateSRV
                )
            );

        // Create an intermediate render target that can be used to render to a texture
        backBufferDesc.Format = m_IntermediateRTFormat;
        backBufferDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &backBufferDesc,
                nullptr,
                &m_spIntermediateRTDebug
                )
            );

        // Create the LuminanceBuffer
        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &backBufferDesc,
                nullptr,
                &m_spLuminanceBufferRT
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spLuminanceBufferRT.Get(),
                &RTVDesc,
                &m_spLuminanceBufferRTV
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spLuminanceBufferRT.Get(),
                &SRVDesc,
                &m_spLuminanceBufferSRV
            )
        );

        // Create the GradientBuffer
        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &backBufferDesc,
                nullptr,
                &m_spXYDirectionBufferRT
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spXYDirectionBufferRT.Get(),
                &RTVDesc,
                &m_spXYDirectionBufferRTV
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spXYDirectionBufferRT.Get(),
                &SRVDesc,
                &m_spXYDirectionBufferSRV
            )
        );

        // Create the stroke direction buffer
        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &backBufferDesc,
                nullptr,
                &m_spStrokeDirectionBufferRT
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spStrokeDirectionBufferRT.Get(),
                &RTVDesc,
                &m_spStrokeDirectionBufferRTV
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spStrokeDirectionBufferRT.Get(),
                &SRVDesc,
                &m_spStrokeDirectionBufferSRV
            )
        );

        // Create the downsampled texture resources
        D3D11_TEXTURE2D_DESC halfResBackBuffer = backBufferDesc;
        halfResBackBuffer.Height /= 4;
        halfResBackBuffer.Width /= 4;

        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &halfResBackBuffer,
                nullptr,
                &m_spQuarterResBufferRT
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spQuarterResBufferRT.Get(),
                &RTVDesc,
                &m_spQuarterResBufferRTV
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spQuarterResBufferRT.Get(),
                &SRVDesc,
                &m_spQuarterResBufferSRV
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateTexture2D(
                &halfResBackBuffer,
                nullptr,
                &m_spQuarterResBufferRT2
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spQuarterResBufferRT2.Get(),
                &RTVDesc,
                &m_spQuarterResBufferRTV2
            )
        );
        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spQuarterResBufferRT2.Get(),
                &SRVDesc,
                &m_spQuarterResBufferSRV2
            )
        );

        // Create the color and specular intensity render target view
        DXThrowIfFailed(
            m_spD3DDevice1->CreateRenderTargetView(
                m_spIntermediateRTDebug.Get(),
                &RTVDesc,
                &m_spIntermediateRTVDebug
                )
            );
        
        // Create the color and specular intensity shader resource view
        DXThrowIfFailed(
            m_spD3DDevice1->CreateShaderResourceView(
                m_spIntermediateRTDebug.Get(),
                &SRVDesc,
                &m_spIntermediateSRVDebug
                )
            );
        
        // Set the 3D rendering viewport to target the entire window.
        m_Viewport.Width = static_cast<float>(backBufferDesc.Width);
        m_Viewport.Height = static_cast<float>(backBufferDesc.Height);

        /*if(!g_WINDOWSYSTEM->m_FullScreen){
            RECT R = { 0, 0, m_Viewport.Width, m_Viewport.Height };
		    AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
            m_Viewport.Width  = R.right - R.left;
            m_Viewport.Height = R.bottom - R.top;
        }*/

        m_spD3DDeviceContext1->RSSetViewports(1, &m_Viewport);
        
#ifdef D2D_ENABLED
        // Create a Direct2D target bitmap associated with the
        // swap chain back buffer and set it as the current target.
        D2D1_BITMAP_PROPERTIES1 bitmapProperties =
            D2D1::BitmapProperties1(
                D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE),
                m_DPIX,
                m_DPIY
            );
        ComPtr<IDXGISurface2> dxgiBackBuffer;
        //spBackBuffer->QueryInterface(__uuidof(IDXGISurface2), (void**)&dxgiBackBuffer);
        DXThrowIfFailed(
            m_spSwapChain1->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer))
            );

        DXThrowIfFailed(
            m_spD2DDeviceContext->CreateBitmapFromDxgiSurface(
                dxgiBackBuffer.Get(),
                &bitmapProperties,
                &m_spD2DTargetBitmap
                )
            );

        m_spD2DDeviceContext->SetTarget(m_spD2DTargetBitmap.Get());

        // Grayscale text anti-aliasing is recommended for all Windows Store apps.
        m_spD2DDeviceContext->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);
#endif
        // Initialize the projection matrix
        //RECT rc;
        GetClientRect(m_Window->m_hwnd, &rc);
        float width = (float)(rc.right - rc.left);
        float height = (float)(rc.bottom - rc.top);
        width *= (1920.0f / g_WINDOWSYSTEM->m_Resx);
        height *= (1080.0f / g_WINDOWSYSTEM->m_Resy);

        XMStoreFloat4x4(&m_Projection, XMMatrixPerspectiveFovLH(XM_PIDIV2, (FLOAT)width / (FLOAT)height, 0.01f, 100.0f));
    }

    void GraphicsSystem::UpdateForWindowSizeChange(){
        CreateWindowSizeDependentResources();
    }

    void GraphicsSystem::CreatePixelShader(wchar_t* fxFileName, LPCSTR pixelShaderEntryPoint, ComPtr<ID3D11PixelShader> &spPixelShader)
    {
        // Compile the pixel shader
        ComPtr<ID3DBlob> spPSBlob;
        CompileShaderFromFile(fxFileName, pixelShaderEntryPoint, "ps_5_0", spPSBlob);

        // Create the pixel shader
        DXThrowIfFailed(
            m_spD3DDevice1->CreatePixelShader(spPSBlob->GetBufferPointer(), spPSBlob->GetBufferSize(), nullptr, spPixelShader.GetAddressOf())
        );
    }

    void GraphicsSystem::CreateShaders(wchar_t* fxFileName, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements, ComPtr<ID3D11InputLayout> &spVertexLayout, LPCSTR vertexShaderEntryPoint, ComPtr<ID3D11VertexShader> &spVertexShader, LPCSTR pixelShaderEntryPoint, ComPtr<ID3D11PixelShader> &spPixelShader){
	    // Compile the vertex shader
	    ComPtr<ID3DBlob> spVSBlob;
        CompileShaderFromFile(fxFileName, vertexShaderEntryPoint, "vs_5_0", spVSBlob);

	    // Create the vertex shader
	    DXThrowIfFailed(
		    m_spD3DDevice1->CreateVertexShader(
			    spVSBlob->GetBufferPointer(),
			    spVSBlob->GetBufferSize(),
			    nullptr, spVertexShader.GetAddressOf())
		    );

		if(layout != NULL)
        {
	        // Create the input layout
	        DXThrowIfFailed(
		        m_spD3DDevice1->CreateInputLayout(
			        layout,
			        numElements,
			        spVSBlob->GetBufferPointer(),
			        spVSBlob->GetBufferSize(),
                    spVertexLayout.GetAddressOf()
			        )
		        );
        }
		
        CreatePixelShader(fxFileName, pixelShaderEntryPoint, spPixelShader);
    }

    void GraphicsSystem::CreateBuffers(){	    
		// Set primitive topology
		m_spD3DDeviceContext1->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        
		// Create the constant buffers
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

        // Create the per frame constant buffer
		bd.ByteWidth = (sizeof(ConstantBufferPerFrame) + 15) / 16 * 16;
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		DXThrowIfFailed(
			m_spD3DDevice1->CreateBuffer(
				&bd,
				nullptr,
                m_spConstantBufferPerFrame.GetAddressOf()
				)
			);
        
        // Create the per object vertex shader constant buffer
        bd.ByteWidth = (sizeof(ConstantBufferPerObjectVS) + 15) / 16 * 16;
		DXThrowIfFailed(
			m_spD3DDevice1->CreateBuffer(
				&bd,
				nullptr,
                m_spConstantBufferPerObjectVS.GetAddressOf()
				)
			);
        
        // Create the per object pixel shader constant buffer
        bd.ByteWidth = (sizeof(ConstantBufferPerObjectPS) + 15) / 16 * 16;
		DXThrowIfFailed(
			m_spD3DDevice1->CreateBuffer(
				&bd,
				nullptr,
                m_spConstantBufferPerObjectPS.GetAddressOf()
				)
			);

        // Create the per sprite constant buffer
        bd.ByteWidth = (sizeof(ConstantBufferPerSpriteObject) + 15) / 16 * 16;
		DXThrowIfFailed(
			m_spD3DDevice1->CreateBuffer(
				&bd,
				nullptr,
                m_spConstantBufferPerSpriteObject.GetAddressOf()
				)
			);

        // Create the GBuffer unpacking constant buffer
        bd.ByteWidth = (sizeof(ConstantBufferGBufferUnpack) + 15) / 16 * 16;
		DXThrowIfFailed(
			m_spD3DDevice1->CreateBuffer(
				&bd,
				nullptr,
                m_spConstantBufferGBufferUnpack.GetAddressOf()
				)
			);

        // Create Vertex buffer for full screen quad
        // Create vertex buffer
	    FullScreenQuadVertex vertices[] =
        {
            { XMFLOAT4(-1.0f,  1.0f, 0.5f, 1.0f), XMFLOAT2(0.0f, 0.0f) }, // Blue (top left)
            { XMFLOAT4( 1.0f,  1.0f, 0.5f, 1.0f), XMFLOAT2(1.0f, 0.0f) }, // Green (top right)
            { XMFLOAT4(-1.0f, -1.0f, 0.5f, 1.0f), XMFLOAT2(0.0f, 1.0f) }, // Pink (bottom left)
            { XMFLOAT4( 1.0f, -1.0f, 0.5f, 1.0f), XMFLOAT2(1.0f, 1.0f) }, // Yellow (bottom right)
	    };

	    ZeroMemory(&bd, sizeof(bd));
	    bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(FullScreenQuadVertex) * ARRAYSIZE(vertices);
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	    bd.CPUAccessFlags = 0;

	    D3D11_SUBRESOURCE_DATA InitData;
	    ZeroMemory(&InitData, sizeof(InitData));
	    InitData.pSysMem = vertices;
	    DXThrowIfFailed(
		    m_spD3DDevice1->CreateBuffer(
			    &bd, 
			    &InitData, 
                m_spFullScreenQuadVertexBuffer.GetAddressOf()));

        // Create index buffer
	    WORD indices[] =
	    {
            // Quad
		    0, 3, 2,
		    1, 3, 0,
	    };
	    bd.Usage = D3D11_USAGE_DEFAULT;
	    bd.ByteWidth = sizeof(WORD)* ARRAYSIZE(indices);
	    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	    bd.CPUAccessFlags = 0;
	    InitData.pSysMem = indices;
	    DXThrowIfFailed(
		    m_spD3DDevice1->CreateBuffer(&bd, &InitData, m_spFullScreenQuadIndexBuffer.ReleaseAndGetAddressOf()));
	}

    void GraphicsSystem::CreateSamplers(){
        // Create the sample state
        D3D11_SAMPLER_DESC sampDesc;
        ZeroMemory( &sampDesc, sizeof(sampDesc) );
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        DXThrowIfFailed(
            m_spD3DDevice1->CreateSamplerState( &sampDesc, m_spLinearSampler.GetAddressOf())
            );

        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        DXThrowIfFailed(
            m_spD3DDevice1->CreateSamplerState(&sampDesc, m_spMirrorSampler.GetAddressOf())
        );

        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        DXThrowIfFailed(
            m_spD3DDevice1->CreateSamplerState( &sampDesc, m_spPointSampler.GetAddressOf())
            );

        D3D11_BLEND_DESC1 BlendState;

        ZeroMemory(&BlendState, sizeof(D3D11_BLEND_DESC1));
        BlendState.RenderTarget[0].BlendEnable = TRUE;
        BlendState.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
        BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
        BlendState.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
        BlendState.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        BlendState.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        BlendState.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
        BlendState.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
        m_spD3DDevice1->CreateBlendState1(&BlendState, &m_spBlendStateEnable);

        //BlendState.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_

        BlendState.RenderTarget[0].BlendEnable = FALSE;
        m_spD3DDevice1->CreateBlendState1(&BlendState, &m_spBlendStateDisable);
    }

	void GraphicsSystem::InitializeMatrices(){
		// Initialize the world matrix
        XMStoreFloat4x4(&m_World, XMMatrixIdentity());

		// Initialize the view matrix
		XMVECTOR Eye = XMVectorSet(0.0f, 25.0f, -25.0f, 0.0f);
		XMVECTOR At = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		XMVECTOR Up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMStoreFloat4x4(&m_View, XMMatrixLookAtLH(Eye, At, Up));
                
        m_Rotate = 0.0f;
    }

    void GraphicsSystem::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ComPtr<ID3DBlob> &m_spBlobOut){
		HRESULT hr = S_OK;

		DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		// Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
		// Setting this flag improves the shader debugging experience, but still allows 
		// the shaders to be optimized and to run exactly the way they will run in 
		// the release configuration of this program.
		dwShaderFlags |= D3DCOMPILE_DEBUG;
        dwShaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

		ComPtr<ID3DBlob> spErrorBlob;
		hr = D3DCompileFromFile(szFileName, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE, szEntryPoint, szShaderModel,
			dwShaderFlags, 0, &m_spBlobOut, &spErrorBlob);
		if (FAILED(hr))
		{
			if (spErrorBlob)
			{
				OutputDebugStringA(reinterpret_cast<const char*>(spErrorBlob->GetBufferPointer()));
			}
			DXThrowIfFailed(hr);
		}
	}

    void GraphicsSystem::CreateBrushes(){
#ifdef D2D_ENABLED
        m_spD2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::White),
            m_spWhiteBrush.GetAddressOf()
            );

        m_spD2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Red),
            m_spRedBrush.GetAddressOf()
            );

        m_spD2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Green),
            m_spGreenBrush.GetAddressOf()
            );

        m_spD2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Blue),
            m_spBlueBrush.GetAddressOf()
            );

        m_spD2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Orange),
            m_spOrangeBrush.GetAddressOf()
            );

        m_spD2DDeviceContext->CreateSolidColorBrush(
            D2D1::ColorF(D2D1::ColorF::Yellow),
            m_spYellowBrush.GetAddressOf()
            );
#endif
    }

    const ComPtr<ID2D1SolidColorBrush> &GraphicsSystem::GetD2DBrush(Color color){
        switch (color)
        {
        case ColorWhite:
            return m_spWhiteBrush;
        case ColorRed:
            return m_spRedBrush;
        case ColorGreen:
            return m_spGreenBrush;
        case ColorBlue:
            return m_spBlueBrush;
        case ColorOrange:
            return m_spOrangeBrush;
        case ColorYellow:
            return m_spYellowBrush;
        default:
            ThrowErrorIf(true, "Brush of that color not found");
            return nullptr;
        }
    }

    void GraphicsSystem::LoadModels(){
        tinyxml2::XMLDocument txmlDoc;
        ThrowErrorIf(
            tinyxml2::XML_SUCCESS != txmlDoc.LoadFile("Assets\\Models\\Models.xml"), 
            "Failed to load Assets\\Models\\Models.xml"
            );
                
        // Loop through all the models and load each one
        tinyxml2::XMLElement* modelElement = txmlDoc.FirstChildElement("Model");
        while (modelElement != nullptr)
        {   
            Model newModel;
            newModel.LoadBinaryModel(modelElement->Attribute("Path"), m_spD3DDevice1);
            //ThrowErrorIf(newModel == NULL, "Model did not load");
            
            /*
            // Open up the model file
            tinyxml2::XMLDocument doc;
            doc.LoadFile(modelElement->Attribute("Path"));

            tinyxml2::XMLElement* txmlElement = doc.FirstChildElement();
            txmlElement->NextSiblingElement();

            VertexType vt;
            LoadXMLAttribute(txmlElement, vt, "vertexLayout");

            switch(vt)
            {
                case VertexTypeAlbedoModel:
                    LoadAlbedoModel(
                        txmlElement,
                        modelElement->Attribute("Name")
                        );
                    break;
                case VertexTypeTexturedModel:
                    LoadTexturedModel(
                        txmlElement,
                        modelElement->Attribute("Name")
                        );
                    break;
                default:
                    ThrowErrorIf(true, "vertexLayout attribute not in model's xml file");
            }
            */
            modelElement = modelElement->NextSiblingElement("Model");
        }
    }    

    void GraphicsSystem::LoadAlbedoModel(tinyxml2::XMLElement* txmlElement, std::string ModelName){
        AlbedoModel m;

        // Get the number of vertices
        m.NumVertices = 24;
        LoadXMLAttribute(txmlElement, m.NumVertices, "numVertices");
        
        // Create the vertex array
        m.VertexArray = new AlbedoModelVertex[m.NumVertices];
        tinyxml2::XMLNode* txmlNode;
        txmlNode = txmlElement;
        for(int i=0; i<m.NumVertices; ++i)
        {
            // Get the position
            txmlNode = txmlNode->NextSibling();
            txmlElement = txmlNode->FirstChildElement();
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Pos.x, "x");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Pos.y, "y");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Pos.z, "z");
            
            // Get the Normal
            txmlElement = txmlElement->NextSiblingElement();
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Normal.x, "x");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Normal.y, "y");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Normal.z, "z");
            
            // Get the Albedo
            txmlElement = txmlElement->NextSiblingElement();
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Albedo.x, "r");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Albedo.y, "g");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Albedo.z, "b");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Albedo.w, "a");
        }

        m_AlbedoModelMap.insert(std::pair<std::string, AlbedoModel>(ModelName, m));
    }

    void GraphicsSystem::LoadTexturedModel(tinyxml2::XMLElement* txmlElement, std::string ModelName){
        TexturedModel m;

        // Get the number of vertices
        m.NumVertices = 24;
        LoadXMLAttribute(txmlElement, m.NumVertices, "numVertices");
        
        // Create the vertex array
        m.VertexArray = new TexturedModelVertex[m.NumVertices];
        tinyxml2::XMLNode* txmlNode;
        txmlNode = txmlElement;
        for(int i=0; i<m.NumVertices; ++i)
        {
            // Get the position
            txmlNode = txmlNode->NextSibling();
            txmlElement = txmlNode->FirstChildElement();
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Pos.x, "x");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Pos.y, "y");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Pos.z, "z");
            
            // Get the Normal
            txmlElement = txmlElement->NextSiblingElement();
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Normal.x, "x");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Normal.y, "y");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].Normal.z, "z");
            
            // Get the Albedo
            txmlElement = txmlElement->NextSiblingElement();
            LoadXMLAttribute(txmlElement, m.VertexArray[i].TextureUV.x, "u");
            LoadXMLAttribute(txmlElement, m.VertexArray[i].TextureUV.y, "v");
        }

        m_TexturedModelMap.insert(std::pair<std::string, TexturedModel>(ModelName, m));
    }

    void GraphicsSystem::LoadTextures(){
        tinyxml2::XMLDocument txmlDoc;
        ThrowErrorIf(
            tinyxml2::XML_SUCCESS != txmlDoc.LoadFile("Assets\\Textures\\Textures.xml"), 
            "Failed to load Assets\\Textures\\Textures.xml"
            );
                
        // Loop through all the textures and load each one
        tinyxml2::XMLElement* textureElement = txmlDoc.FirstChildElement("Texture");
        while (textureElement != nullptr)
        {
            std::string strPath = textureElement->Attribute("Path");
            
            // If it is a .dds file
            if(strPath.find(".dds") == strPath.size() - 4)
            {
                LoadTexture(
                    textureElement->Attribute("Name"),
                    textureElement->Attribute("Path")
                    );
            }
            else
            {
                LoadBitmapFromFile(
                    textureElement->Attribute("Name"),
                    ConvertStringToWString(strPath)
                    );
            }

            textureElement = textureElement->NextSiblingElement("Texture");
        }
    }

    void GraphicsSystem::LoadTexture(std::string TextureName, std::string TextureFilePath){
        ComPtr<ID3D11ShaderResourceView> spSRV;

        // For starters, always load the same, hard coded texture
        // Will eventually create this object using metadata
        wchar_t filePath[MAX_PATH];
        swprintf(filePath, MAX_PATH, L"%hs", TextureFilePath.c_str());

        //Use D3DX to load the texture
        DXThrowIfFailed(
            CreateDDSTextureFromFile(
                m_spD3DDevice1.Get(),
                filePath,
                nullptr,
                &spSRV
                )
            );
        
        //TODO: handle textures with different paths but the same name
        //Add to Graphics list of textures: Textures[texturefile.FileName.c_str()] = newTexture;
        //m_TextureMap[std::string("Default")] = spSRV;
        m_TextureMap.insert(std::pair<std::string, ComPtr<ID3D11ShaderResourceView> >(TextureName, spSRV));
    }

    
    void GraphicsSystem::LoadBitmapFromFile(std::string BitmapName, std::wstring BitmapFilePath){
        // Add the bitmap to the bitmap map
        m_BitmapMap.insert(std::pair<std::string, ComPtr<ID2D1Bitmap1> >(BitmapName, CreateBitmapFromFile(BitmapFilePath)));
    }

    ComPtr<ID2D1Bitmap1> GraphicsSystem::CreateBitmapFromFile(std::wstring BitmapFilePath)
    {
        return CreateBitmapFromFile(BitmapFilePath, D2D1_BITMAP_OPTIONS_NONE);
    }

    ComPtr<ID2D1Bitmap1> GraphicsSystem::CreateBitmapFromFile(std::wstring BitmapFilePath, D2D1_BITMAP_OPTIONS options) {
        // Create the decoder
        ComPtr<IWICBitmapDecoder> spDecoder;
        DXThrowIfFailed(
            m_spWICFactory->CreateDecoderFromFilename(
                BitmapFilePath.c_str(),
                NULL,
                GENERIC_READ,
                WICDecodeMetadataCacheOnLoad,
                &spDecoder
            )
        );

        // Create the initial frame
        ComPtr<IWICBitmapFrameDecode> spSource;
        DXThrowIfFailed(
            spDecoder->GetFrame(0, &spSource)
        );

        // Convert the image format to 32bppPBGRA
        ComPtr<IWICFormatConverter> spConverter;
        DXThrowIfFailed(
            m_spWICFactory->CreateFormatConverter(&spConverter)
        );

        DXThrowIfFailed(
            spConverter->Initialize(
                spSource.Get(),
                GUID_WICPixelFormat32bppBGR,
                WICBitmapDitherTypeNone,
                NULL,
                0.0f,
                WICBitmapPaletteTypeMedianCut
            )
        );

        // Create a Direct2D bitmap from the WIC bitmap
        ComPtr<ID2D1Bitmap1> spBitmap;

        D2D1_BITMAP_PROPERTIES1 properties;
        properties.dpiX = m_DPIX;
        properties.dpiY = m_DPIY;
        properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        properties.bitmapOptions = options;
        properties.colorContext = nullptr;
        DXThrowIfFailed(
            m_spD2DDeviceContext->CreateBitmapFromWicBitmap(
                spConverter.Get(),
                properties,
                spBitmap.GetAddressOf()
            )
        );

        return spBitmap;
    }
    
    ComPtr<ID2D1Bitmap1> GraphicsSystem::CreateWriteableBitmap(D2D1_SIZE_U size)
    {
        ComPtr<ID2D1Bitmap1> spBitmap;

        D2D1_BITMAP_PROPERTIES1 properties;
        properties.dpiX = m_DPIX;
        properties.dpiY = m_DPIY;
        properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        properties.colorContext = nullptr;
        DXThrowIfFailed(
            m_spD2DDeviceContext->CreateBitmap(
                size,
                nullptr,
                0,
                properties,
                spBitmap.GetAddressOf()                
            )
        );

        return spBitmap;
    }

    ComPtr<ID2D1Bitmap1> GraphicsSystem::CreateBitmapCopy(ComPtr<ID2D1Bitmap1> sourceBitmap, D2D1_BITMAP_OPTIONS options)
    {
        ComPtr<ID2D1Bitmap1> destinationBitmap;
        D2D1_BITMAP_PROPERTIES1 properties;
        properties.dpiX = m_DPIX;
        properties.dpiY = m_DPIY;
        properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        properties.bitmapOptions = options;
        properties.colorContext = nullptr;

        DXThrowIfFailed(
            m_spD2DDeviceContext->CreateBitmap(
                sourceBitmap.Get()->GetPixelSize(),
                nullptr,
                0,
                &properties,
                destinationBitmap.GetAddressOf()
            ));

        DXThrowIfFailed(
            destinationBitmap->CopyFromBitmap(nullptr, sourceBitmap.Get(), nullptr)
        );

        return destinationBitmap;
    }

    ComPtr<ID2D1Bitmap1> GraphicsSystem::CreateReadableBitmapCopy(ComPtr<ID2D1Bitmap1> sourceBitmap)
    {
        return CreateBitmapCopy(sourceBitmap, D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW);
    }

    const AlbedoModel* GraphicsSystem::GetAlbedoModel(std::string ModelName){
        std::unordered_map<std::string, AlbedoModel >::iterator it = m_AlbedoModelMap.find(ModelName);
        if (it != m_AlbedoModelMap.end())
            return &it->second;
        else
            return NULL;
    }

    const TexturedModel* GraphicsSystem::GetTexturedModel(std::string ModelName){
        std::unordered_map<std::string, TexturedModel >::iterator it = m_TexturedModelMap.find(ModelName);
        if (it != m_TexturedModelMap.end())
            return &it->second;
        else
            return NULL;
    }

    const ComPtr<ID3D11ShaderResourceView> GraphicsSystem::GetTexture(std::string TextureName){
        std::unordered_map<std::string, ComPtr<ID3D11ShaderResourceView> >::iterator it = m_TextureMap.find(TextureName);
        if (it != m_TextureMap.end())
            return it->second;
        else
            return NULL;
    }

    const ComPtr<ID2D1Bitmap1> GraphicsSystem::GetBitmap(std::string BitmapName){
        std::unordered_map<std::string, ComPtr<ID2D1Bitmap1> >::iterator it = m_BitmapMap.find(BitmapName);
        if (it != m_BitmapMap.end())
            return it->second;
        else
            return NULL;
    }

    const ComPtr<ID3D11InputLayout>& GraphicsSystem::GetVertexLayout(VertexType vt){
        switch(vt)
        {
        case VertexTypeFBXBinModel:
            return m_spFBXBinModelVertexLayout;
        case VertexTypeFBXBinSkinnedModel:
            return m_spFBXBinSkinnedModelVertexLayout;
        case VertexTypeAlbedoModel:
            return m_spAlbedoModelVertexLayout;
        case VertexTypeTexturedModel:
            return m_spTexturedModelVertexLayout;
        default:
            ThrowErrorIf(true, "VertexLayout not recognized");
            return nullptr;
        }
    }
    const ComPtr<ID3D11VertexShader>& GraphicsSystem::GetGBufferVertexShader(VertexType vt){
        switch(vt)
        {
        case VertexTypeFBXBinModel:
            return m_spGBufferFBXBinModelVertexShader;
        case VertexTypeFBXBinSkinnedModel:
            return m_spGBufferFBXBinSkinnedModelVertexShader;
        case VertexTypeAlbedoModel:
            return m_spGBufferAlbedoModelVertexShader;
        case VertexTypeTexturedModel:
            return m_spGBufferTexturedModelVertexShader;
        default:
            return m_spGBufferAlbedoModelVertexShader;
        }
    }
    const ComPtr<ID3D11PixelShader>& GraphicsSystem::GetGBufferPixelShader(VertexType vt){
        switch(vt)
        {
        case VertexTypeFBXBinModel:
            return m_spGBufferFBXBinModelPixelShader;
        case VertexTypeFBXBinSkinnedModel:
            return m_spGBufferFBXBinSkinnedModelPixelShader;
        case VertexTypeAlbedoModel:
            return m_spGBufferAlbedoModelPixelShader;
        case VertexTypeTexturedModel:
            return m_spGBufferTexturedModelPixelShader;
        default:
            return m_spGBufferAlbedoModelPixelShader;
        }
    }

    XMFLOAT4X4 GraphicsSystem::GetViewTransform(){return *(m_pCurrentCamera->GetView());}

    // Add Components to Lists
    void GraphicsSystem::AddModelToList(Model* pModel){
        m_ModelList.push_back(pModel);
    }

    void GraphicsSystem::AddSpriteToList(Sprite* pSprite){
        m_SpriteList.push_back(pSprite);
    }

    void GraphicsSystem::AddSpriteToBackgroundList(Sprite* pSprite){
        m_BackgroundSpriteList.push_back(pSprite);
    }

    void GraphicsSystem::AddCameraToList(Camera* pCamera){
        m_CameraList.push_back(pCamera);
    }

    void GraphicsSystem::AddTextToList(Text* pText){
        m_TextList.push_back(pText);
    }

    void GraphicsSystem::AddImageToList(Image* pImage){
        m_ImageList.push_back(pImage);
    }

	void GraphicsSystem::DrawTriggers()
	{
		std::list<Sprite*>::iterator it = m_SpriteList.begin();
		for( ; it != m_SpriteList.end(); ++it)
		{
			Trigger * tc = (*it)->GetOwner()->has(Trigger);
			if(tc)
				(*it)->TurnDrawingOn();
		}
	}
	
	void GraphicsSystem::HideTriggers()
	{
		std::list<Sprite*>::iterator it = m_SpriteList.begin();
		for( ; it != m_SpriteList.end(); ++it)
		{
			Trigger * tc = (*it)->GetOwner()->has(Trigger);
			if(tc)
			{
				if(tc->GetOwner()->GetType() != "train")
					(*it)->TurnDrawingOff();
			}
		}
	}

    XMFLOAT2 GraphicsSystem::ConvertToScreenCoordinates(XMFLOAT3 worldPosition, XMFLOAT4X4 worldProjection)
    {
        XMFLOAT2 screenCoordinate;
        XMStoreFloat2(
            &screenCoordinate, 
            DirectX::XMVector3Project(
                XMLoadFloat3(&worldPosition),
                m_Viewport.TopLeftX,
                m_Viewport.TopLeftY,
                m_Viewport.Width,
                m_Viewport.Height,
                m_Viewport.MinDepth,
                m_Viewport.MaxDepth,
                XMLoadFloat4x4(&m_Projection),
                XMLoadFloat4x4(m_pCurrentCamera->GetView()),
                XMLoadFloat4x4(&worldProjection)
                )
            );
        
        return screenCoordinate;
    }

    XMFLOAT3 GraphicsSystem::ConvertToWorldCoordinates(XMFLOAT2 screenCoordinate, float zPlane)
    {
        // Build the plane
        XMFLOAT3 p0(0.0f, 0.0f, zPlane);
        XMFLOAT3 p1(0.0f, 1.0f, zPlane);
        XMFLOAT3 p2(1.0f, 1.0f, zPlane);
        
        return ConvertToWorldCoordinates(screenCoordinate, p0, p1, p2);
    }

    XMFLOAT3 GraphicsSystem::ConvertToWorldCoordinates(XMFLOAT2 screenCoordinate, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2)
    {
        // Get two points along the ray in world coordinates
        XMFLOAT3 mouseCoordinateNear(screenCoordinate.x * m_Viewport.Width, screenCoordinate.y * m_Viewport.Height, 0.0f);
        XMFLOAT3 mouseCoordinateFar(screenCoordinate.x * m_Viewport.Width, screenCoordinate.y * m_Viewport.Height, 1.0f);
        XMFLOAT3 rayCoordinate0;
        XMStoreFloat3(
            &rayCoordinate0,
            XMVector3Unproject(
            XMLoadFloat3(&mouseCoordinateNear),
            m_Viewport.TopLeftX,
            m_Viewport.TopLeftY,
            m_Viewport.Width,
            m_Viewport.Height,
            m_Viewport.MinDepth,
            m_Viewport.MaxDepth,
            XMLoadFloat4x4(&m_Projection),
            XMLoadFloat4x4(m_pCurrentCamera->GetView()),
            XMLoadFloat4x4(&m_World)
            )
            );

        XMFLOAT3 rayCoordinate1;
        XMStoreFloat3(
            &rayCoordinate1,
            XMVector3Unproject(
            XMLoadFloat3(&mouseCoordinateFar),
            m_Viewport.TopLeftX,
            m_Viewport.TopLeftY,
            m_Viewport.Width,
            m_Viewport.Height,
            m_Viewport.MinDepth,
            m_Viewport.MaxDepth,
            XMLoadFloat4x4(&m_Projection),
            XMLoadFloat4x4(m_pCurrentCamera->GetView()),
            XMLoadFloat4x4(&m_World)
            )
            );

        // Calculate the intersect of the desired plane and the ray
        XMFLOAT3 worldCoordinate;
        XMStoreFloat3(
            &worldCoordinate,
            XMPlaneIntersectLine(
            XMPlaneFromPoints(
            XMLoadFloat3(&p0),
            XMLoadFloat3(&p1),
            XMLoadFloat3(&p2)
            ),
            XMLoadFloat3(&rayCoordinate0),
            XMLoadFloat3(&rayCoordinate1)
            )
            );

        return worldCoordinate;
    }

    void GraphicsSystem::ToggleClosestAnchor(XMFLOAT2 mousePos)
    {
        // Bail early if anchor points is empty
        if (g_PHYSICSSYSTEM->m_AnchorPoints.size() == 0)
        {
            return;
        }
        
        // Loop through all the points, storing the index of the closest point
        std::list<UINT>::iterator it = g_PHYSICSSYSTEM->m_AnchorPoints.begin();
        std::list<UINT>::iterator closestIt = it;
        float closestDist = FLT_MAX;

        for (; it != g_PHYSICSSYSTEM->m_AnchorPoints.end(); ++it)
        {
            // Get the distance between the cursor and the point
            XMFLOAT2 anchorPos = ConvertToScreenCoordinates(g_PHYSICSSYSTEM->m_rigidBodies[*it].x, m_World);
            float dist = Mag(mousePos - anchorPos);
            if (dist < closestDist)
            {
                closestIt = it;
                closestDist = dist;
            }
        }

        g_PHYSICSSYSTEM->m_AnchorPoints.erase(closestIt);
    }

    void GraphicsSystem::ToggleClosestPoint(XMFLOAT2 mousePos)
    {
        // If the max number of anchor points have already been reached, bail early
        if (g_PHYSICSSYSTEM->m_AnchorPoints.size() > g_PHYSICSSYSTEM->m_MaxAnchorPoints)
        {
            return;
        }

        UINT closestPoint = 0;
        float closestDist = FLT_MAX;
        // Loop through all the anchor points, storing the index of the closest point
        for (UINT i = 0; i < NBODIES; ++i)
        {
            // Get the distance between the cursor and the point
            XMFLOAT2 pointPos = ConvertToScreenCoordinates(g_PHYSICSSYSTEM->m_rigidBodies[i].x, m_World);
            float dist = Mag(mousePos - pointPos);
            if (dist < closestDist)
            {
                closestPoint = i;
                closestDist = dist;
            }
        }

        // Add the closest point
        g_PHYSICSSYSTEM->m_AnchorPoints.push_back(closestPoint);
        // Make sure it's not a duplicate
        g_PHYSICSSYSTEM->m_AnchorPoints.unique();
        g_PHYSICSSYSTEM->m_rigidBodies[closestPoint].force = XMFLOAT3(0.f, 0.f, 0.f);
        g_PHYSICSSYSTEM->m_rigidBodies[closestPoint].R = XMFLOAT3X3(0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f, 0.f);
        g_PHYSICSSYSTEM->m_rigidBodies[closestPoint].L = XMFLOAT3(0.f, 0.f, 0.f);
        g_PHYSICSSYSTEM->m_rigidBodies[closestPoint].P = XMFLOAT3(0.f, 0.f, 0.f);
    }

    void GraphicsSystem::DrawBodies(rBody *rBodies, UINT length)
    {
        m_spD2DDeviceContext->BeginDraw();

        // Draw the mass points
        for (UINT i = 0; i < length; ++i)
        {
            XMFLOAT2 point = ConvertToScreenCoordinates(rBodies[i].x, m_World);

            if (!g_PHYSICSSYSTEM->IsAnchor(i))
            {
                m_spD2DDeviceContext->DrawEllipse(
                    D2D1::Ellipse(
                    D2D1::Point2F(point.x, point.y),
                    1.5f, // Hard coded radius of 2 for now
                    2.f),
                    GetD2DBrush(ColorGreen).Get());
            }
            else
            {
                m_spD2DDeviceContext->DrawEllipse(
                    D2D1::Ellipse(
                    D2D1::Point2F(point.x, point.y),
                    3.f, // Hard coded radius of 2 for now
                    3.f),
                    GetD2DBrush(ColorRed).Get());
            }
        }

        // Draw the springs
        for (UINT i = 0; i < CUBE_WIDTH; ++i)
        {
            for (UINT j = 0; j < CUBE_HEIGHT; ++j)
            {
                for (UINT k = 0; k < CUBE_DEPTH; ++k)
                {
                    g_PHYSICSSYSTEM->DrawSpringsAtPoint(i, k, j, m_spD2DDeviceContext);
                }
            }
        }

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    void GraphicsSystem::DrawFloor()
    {
        m_spD2DDeviceContext->BeginDraw();

        int min = -10;
        int max = 11;
        float scale = 1.f;
        for (int i = min; i < max; ++i)
        {
            XMFLOAT2 points[4];
            points[0] = ConvertToScreenCoordinates(XMFLOAT3((float)i * scale, 0.0f, (float)min * scale), m_World);
            points[1] = ConvertToScreenCoordinates(XMFLOAT3((float)i * scale, 0.0f, (float)max * scale), m_World);

            points[2] = ConvertToScreenCoordinates(XMFLOAT3((float)min * scale, 0.0f, (float)i * scale), m_World);
            points[3] = ConvertToScreenCoordinates(XMFLOAT3((float)max * scale, 0.0f, (float)i * scale), m_World);

            m_spD2DDeviceContext->DrawLine(
                D2D1::Point2F(points[0].x, points[0].y),
                D2D1::Point2F(points[1].x, points[1].y),
                GetD2DBrush(ColorYellow).Get());


            m_spD2DDeviceContext->DrawLine(
                D2D1::Point2F(points[2].x, points[2].y),
                D2D1::Point2F(points[3].x, points[3].y),
                GetD2DBrush(ColorYellow).Get());
        }

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    //Tonal Art Map
    TAMTexture GraphicsSystem::CreateBitmapCopy(TAMTexture source)
    {
        TAMTexture copy;
        D2D1_BITMAP_PROPERTIES1 properties;
        // TAMTODO: do not force all bitmaps to be CPU_READ
        properties.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
        properties.dpiX = m_DPIX;
        properties.dpiY = m_DPIY;
        properties.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        properties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        properties.colorContext = nullptr;

        m_spD2DDeviceContext->CreateBitmap(
            source.Get()->GetPixelSize(),
            nullptr,
            0,// TAMTODO: calculate proper pitch?
            properties,
            copy.GetAddressOf()
        );

        DXThrowIfFailed(copy.Get()->CopyFromBitmap(nullptr, source.Get(), nullptr));

        /*m_spD2DDeviceContext->SetTarget(copy.Get());
        m_spD2DDeviceContext->BeginDraw();
        m_spD2DDeviceContext->DrawBitmap(source.Get());
        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());*/

        return copy;
    }

    void GraphicsSystem::SaveBitmapToFile(
        ComPtr<ID2D1Bitmap1> d2dBitmap,
        REFGUID wicFormat,
        std::wstring filePath
    )
    {
        ComPtr<IWICStream> stream;
        DXThrowIfFailed(m_spWICFactory->CreateStream(stream.GetAddressOf()));
        DXThrowIfFailed(stream->InitializeFromFilename(filePath.c_str(), GENERIC_WRITE));
        // Create and initialize WIC Bitmap Encoder.
        ComPtr<IWICBitmapEncoder> wicBitmapEncoder;
        DXThrowIfFailed(
            m_spWICFactory->CreateEncoder(
                wicFormat,
                nullptr,    // No preferred codec vendor.
                &wicBitmapEncoder
            )
        );

        DXThrowIfFailed(
            wicBitmapEncoder->Initialize(
                stream.Get(),
                WICBitmapEncoderNoCache
            )
        );

        // Create and initialize WIC Frame Encoder.
        ComPtr<IWICBitmapFrameEncode> wicFrameEncode;
        DXThrowIfFailed(
            wicBitmapEncoder->CreateNewFrame(
                &wicFrameEncode,
                nullptr     // No encoder options.
            )
        );

        DXThrowIfFailed(
            wicFrameEncode->Initialize(nullptr)
        );

        ComPtr<IWICBitmap> wicBitmap;
        DXThrowIfFailed(
            m_spWICFactory->CreateBitmap(d2dBitmap.Get()->GetPixelSize().width, d2dBitmap.Get()->GetPixelSize().height, GUID_WICPixelFormat32bppBGR, WICBitmapCacheOnDemand, &wicBitmap)
        );
        D2D1_RENDER_TARGET_PROPERTIES rtProps = D2D1::RenderTargetProperties();
        rtProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
        rtProps.type = D2D1_RENDER_TARGET_TYPE_DEFAULT;
        rtProps.usage = D2D1_RENDER_TARGET_USAGE_NONE;

        // define the render target
        ComPtr<ID2D1RenderTarget> spRenderTarget = 0;
        DXThrowIfFailed(m_spD2DFactory->CreateWicBitmapRenderTarget(wicBitmap.Get(), rtProps, spRenderTarget.GetAddressOf())
        );
        ComPtr<ID2D1Bitmap> spBitmap;
        DXThrowIfFailed(
            spRenderTarget->CreateBitmap(d2dBitmap.Get()->GetPixelSize(), D2D1::BitmapProperties(d2dBitmap->GetPixelFormat()), &spBitmap)
        );

       /* DXThrowIfFailed(
            spBitmap->CopyFromBitmap(nullptr, d2dBitmap.Get(), nullptr)
            );
        m_spD2DDeviceContext->SetTarget(spBitmap.Get());
        spRenderTarget->BeginDraw();
        spRenderTarget->Clear();
        spRenderTarget->DrawBitmap(spBitmap.Get());
        DXThrowIfFailed(spRenderTarget->EndDraw());*/

        ComPtr<ID2D1Bitmap1> readableD2DBitmap = CreateReadableBitmapCopy(d2dBitmap);
        D2D1_MAP_OPTIONS options = D2D1_MAP_OPTIONS_READ;
        D2D1_MAPPED_RECT mappedRect;
        DXThrowIfFailed(readableD2DBitmap.Get()->Map(options, &mappedRect));

        UINT width = readableD2DBitmap.Get()->GetPixelSize().width;
        UINT height = readableD2DBitmap.Get()->GetPixelSize().height;
        UINT  pitch = 4;
        ComPtr<IWICBitmap> spWICBitmapFromMappedRect;
        DXThrowIfFailed(
            m_spWICFactory->CreateBitmapFromMemory(width, height, GUID_WICPixelFormat32bppBGR, pitch * width, width * height * pitch, reinterpret_cast<BYTE*>(mappedRect.bits), spWICBitmapFromMappedRect.GetAddressOf())
        );
         DXThrowIfFailed(
            wicFrameEncode->WriteSource(wicBitmap.Get(), nullptr)
        );// Create IWICImageEncoder.
        /*ComPtr<IWICImageEncoder> imageEncoder;
        DXThrowIfFailed(
            m_spWICFactory->CreateImageEncoder(
                m_spD2DDevice.Get(),
                &imageEncoder
            )
        );

        DXThrowIfFailed(
            imageEncoder->WriteFrame(
                d2dBitmap.Get(),
                wicFrameEncode.Get(),
                nullptr     // Use default WICImageParameter options.
            )
        );*/

        DXThrowIfFailed(
            wicFrameEncode->Commit()
        );

        DXThrowIfFailed(
            wicBitmapEncoder->Commit()
        );

        // Flush all memory buffers to the next-level storage object.
        /*DXThrowIfFailed(
            stream->Commit(STGC_DEFAULT)
        );*/
    }
    void GraphicsSystem::Set2DTarget(TAMTexture target)
    {
        m_spD2DDeviceContext->SetTarget(target.Get());
        m_spD2DDeviceContext->SetTransform(D2D1::Matrix3x2F::Identity());
    }
    void GraphicsSystem::Begin2DDraw()
    {
        m_spD2DDeviceContext->BeginDraw();
    }
    void GraphicsSystem::End2DDraw()
    {
        DXThrowIfFailed(
            m_spD2DDeviceContext->EndDraw()
        );
    }
    void GraphicsSystem::Clear2DRenderTarget(D2D1_COLOR_F& color)
    {
        m_spD2DDeviceContext->Clear(color);
    }
    void GraphicsSystem::DrawTAMStroke(TAMStroke stroke, TAMTexture target)
    {
        /*ComPtr<ID2D1BitmapBrush1> brushStroke;
        m_spD2DDeviceContext->CreateBitmapBrush(
            stroke.strokeTexture.Get(),
            &brushStroke
            );*/

        m_spD2DDeviceContext->BeginDraw();

        // Figure out the place on the target image the stroke should go
        D2D1_SIZE_F targetSize = target.Get()->GetSize();
        D2D1_RECT_F strokeRect;
        strokeRect.left = stroke.uvCoordinate.x * targetSize.width;
        strokeRect.top = stroke.uvCoordinate.y * targetSize.height;
        strokeRect.right = strokeRect.left + (float)stroke.dimension.x * targetSize.width;
        strokeRect.bottom = strokeRect.top + (float)stroke.dimension.y * targetSize.height;

        // TAMTODO: blend like a pencil would really blend
        m_spD2DDeviceContext->SetPrimitiveBlend(D2D1_PRIMITIVE_BLEND_SOURCE_OVER);

        m_spD2DDeviceContext->DrawBitmap(
            stroke.strokeTexture.Get(),
            strokeRect
        );

        DXThrowIfFailed(m_spD2DDeviceContext->EndDraw());
    }

    void GraphicsSystem::GenerateTAMs(int minMipResolution, int maxDimension, int numTones)
    {
        m_tamGenerator = TonalArtMapGenerator(minMipResolution, maxDimension, numTones);
        m_tamGenerator.GenerateTAM("");
    }
}

