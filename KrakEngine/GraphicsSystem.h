/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: GraphicsSystem.h
Purpose: Header for GraphicsSystem.cpp
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 1/20/2014
- End Header -----------------------------------------------------*/
#pragma once

#include "Precompiled.h"
#include "ISystem.h"
#include "WindowSystem.h"
#include "ObjectLinkedList.h"
#include "Vertex.h"
#include "GBuffer.h"

namespace KrakEngine{
    
    struct SpriteAnimation{
        float Length;
        float NumFrames;
        float StartFrame;
    };

	enum AnimationState{
		AnimationStateIdle,
		AnimationStateRunning,
		AnimationStateJumping,
	};

    enum Color{
        ColorWhite,
        ColorRed,
        ColorGreen,
        ColorBlue,
        ColorOrange,
        ColorYellow
    };

	class Camera;
	class Sprite;
    class Model;
    class Text;
    class RigidBody;
    class Image;
    class AnimationController;
    struct rBody;

    struct ArcLengthTableElement{
        double u;
        double s;
        XMFLOAT2 p;
    };

    class GraphicsSystem : public ISystem{

    public:
        GraphicsSystem();
        GraphicsSystem(WindowSystem* window);
        ~GraphicsSystem();


        virtual bool Initialize();
        virtual bool Shutdown();
        virtual void Update(float dt);
        virtual void HandleMessages(Message* e);

        const ComPtr<ID2D1DeviceContext>& GetD2DDeviceContext(){ return m_spD2DDeviceContext; }
        const ComPtr<ID3D11Device1>& GetD3DDevice1(){ return m_spD3DDevice1; }
        const ComPtr<ID3D11DeviceContext1>& GetD3DDeviceContext1(){ return m_spD3DDeviceContext1; }
        const ComPtr<ID3D11InputLayout>& GetVertexLayout(VertexType vt);
        const ComPtr<ID3D11VertexShader>& GetGBufferVertexShader(VertexType vt);
        const ComPtr<ID3D11PixelShader>& GetGBufferPixelShader(VertexType vt);
        const ComPtr<ID3D11InputLayout>& GetSpriteVertexLayout(){ return m_spSpriteVertexLayout; }
        const ComPtr<ID3D11VertexShader>& GetSpriteVertexShader(){ return m_spSpriteVertexShader; }
        const ComPtr<ID3D11PixelShader>& GetSpritePixelShader(){ return m_spSpritePixelShader; }
        const ComPtr<ID3D11SamplerState>& GetSampler(){ return m_spLinearSampler; }
        XMUINT2 GetWindowSize(){ return m_WindowSize; }
        XMFLOAT4X4 GetViewTransform();
        XMFLOAT4X4 GetProjectionTransform(){ return m_Projection; }
        XMFLOAT2 ConvertToScreenCoordinates(XMFLOAT3 worldPosition, XMFLOAT4X4 worldProjection);
        XMFLOAT3 ConvertToWorldCoordinates(XMFLOAT2 screenCoordinate, float zPlane = 0.0f);
        XMFLOAT3 ConvertToWorldCoordinates(XMFLOAT2 screenCoordinate, XMFLOAT3 p0, XMFLOAT3 p1, XMFLOAT3 p2);

        void ToggleDebug(){ m_DrawDebug = (m_DrawDebug + 1) % 3; }
        void ToggleModel(){ m_ChooseModel = 1 + (m_ChooseModel + 1) % 5; }
        bool IsGBufferCreationOn(){ return m_DrawDebug != 4; }
        bool IsDebugDrawingOn(){ return m_DrawDebug == 1 || m_DrawDebug == 2; }
        bool IsGBufferDrawingOn(){ return m_DrawDebug == 5; }
        bool IsSceneDrawingOn(){ return m_DrawDebug != 5; }
        bool IsMeshDrawingOn(){ return m_DrawDebug == 1 || m_DrawDebug == 2; }
        bool IsSkeletonDrawingOn(){ return m_DrawDebug != 5;}
        bool IsDrawBindPose(){ return m_DrawDebug == 2; }
        bool IsSkinningOn(){ return m_DrawDebug == 1; }
        void LoadModels();
        void LoadAlbedoModel(tinyxml2::XMLElement* txmlElement, std::string ModelName);
        void LoadTexturedModel(tinyxml2::XMLElement* txmlElement, std::string ModelName);
        void LoadTextures();
        void LoadTexture(std::string TextureName, std::string TextureFilePath);
        void LoadBitmapFromFile(std::string BitmapName, std::wstring BitmapFilePath);
        const AlbedoModel* GetAlbedoModel(std::string ModelName);
        const TexturedModel* GetTexturedModel(std::string ModelName);
        const ComPtr<ID3D11ShaderResourceView> GetTexture(std::string TextureName);
        const ComPtr<ID2D1Bitmap1> GetBitmap(std::string TextureName);

        const ComPtr<ID2D1SolidColorBrush> &GetD2DBrush(Color color);

        Camera * GetCurrentCamera() { return m_CameraList.front(); };

        // Add Components to Lists
        void AddModelToList(Model* pModel);
        void AddSpriteToList(Sprite* pSprite);
        void AddSpriteToBackgroundList(Sprite* pSprite);
        void AddCameraToList(Camera* pCamera);
        void AddTextToList(Text* pText);
        void AddImageToList(Image* pImage);

        // Toggle Triggers
        void DrawTriggers();
        void HideTriggers();

        HWND GetHWND() { return m_Window->m_hwnd; }
        void SetFullScreen(bool state){ m_spSwapChain1->SetFullscreenState(state, nullptr); }

        // CS 560 specific stuff
        float GetPathPointRadius(){ return m_PathPointRadius; }
        bool IsEditPathOn(){ return m_bEditPath; }
        void ToggleEditPath(){ m_bEditPath = !m_bEditPath; }
        void DrawMouse();
        std::vector<XMFLOAT3> m_PathControlPoints;
        XMFLOAT4X4 GetWorldTransform(){ return m_World; }
        void SetMousePos(XMFLOAT2 pos){ m_MousePos = pos; }
        void UpdateIKSystem();
        void UpdateLinearSystem();
        XMFLOAT3 m_IKTargetPosition;
        bool m_bInterpolateIK;
        float m_IKAnimationTime;
        void InterpolateIK(float dt, AnimationController *pController);
        float m_IKAnimationLength = 1.f;

        // CS 560 project 4
        void DrawBodies(rBody *rBodies, UINT length);
        void DrawFloor();

    private:
        // Draw Functions
        void DrawFullScreenQuad(ComPtr<ID3D11VertexShader> spVertexShader, ComPtr<ID3D11PixelShader> spPixelShader);
        void DoLighting();
        void DoPostProcessing();
        void UpdateModels(float dt);
        void DrawModels();
        void DrawBones();
        void DrawSelected();
        void UpdateSprites(float dt);
        void UpdateBackgroundSprites(float dt);
        void SortSprites();
        void SortBackgroundSprites();
        void DrawSprites();
        void DrawBackgroundSprites();
        void SortText();
        void DrawTextComponents();
        void UpdateImages(float dt);
        void SortImages();
        void DrawImages();
        //void DrawLines();
        void DrawDebugInfo();
        void DrawDebug(Model* pModel);
        void DrawDebug(Sprite* pSprite);
        void DrawProjectedDebugBox(XMFLOAT4X4 worldTransform, RigidBody* pPhysics);

        void LoadResources();
        // Factory, Device, and Window Resource Creation
        void CreateDeviceIndependentResources();
        void CreateDeviceResources();
        void CreateWindowSizeDependentResources();
        void UpdateForWindowSizeChange();

        // D3D Resource Creation
        void CreateShaders(wchar_t *fxFileName, D3D11_INPUT_ELEMENT_DESC* layout, UINT numElements, ComPtr<ID3D11InputLayout> &spVertexLayout, ComPtr<ID3D11VertexShader> &spVertexShader, ComPtr<ID3D11PixelShader> &spPixelShader);
        void CreateBuffers();
        void CreateQuadResources();
        void CreateSamplers();
        void InitializeMatrices();
        void CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ComPtr<ID3DBlob> &m_spBlobOut);

        // 2D Resource Creation
        void CreateBrushes();

        // Object Management
        void RemoveModelComponent(int objectid);
        void RemoveSpriteComponent(int objectid);
        void RemoveCameraComponent(int objectid);
        void RemoveTextComponent(int objectid);
        void RemoveImageComponent(int objectid);

        // DXGI Resources
        ComPtr<IDXGISwapChain1> m_spSwapChain1;

        // D3D Resources
        ComPtr<ID3D11Device1> m_spD3DDevice1;
        ComPtr<ID3D11DeviceContext1> m_spD3DDeviceContext1;
        GBuffer m_GBuffer;
        ComPtr<ID3D11RenderTargetView> m_spD3DRenderTargetView;
        ComPtr<ID3D11Texture2D> m_spIntermediateRT;
        ComPtr<ID3D11RenderTargetView> m_spIntermediateRTV;
        ComPtr<ID3D11ShaderResourceView> m_spIntermediateSRV;
        DXGI_FORMAT m_IntermediateRTFormat;


        ComPtr<ID3D11Texture2D> m_spIntermediateRTDebug;
        ComPtr<ID3D11RenderTargetView> m_spIntermediateRTVDebug;
        ComPtr<ID3D11ShaderResourceView> m_spIntermediateSRVDebug;
        DXGI_FORMAT m_IntermediateRTFormatDebug;

        //ComPtr<ID3D11DepthStencilView> m_spD3DDepthStencilView;
        D3D_FEATURE_LEVEL m_FeatureLevel;
        D3D_DRIVER_TYPE m_DriverType;

        // GBuffer Layout and Shaders
        ComPtr<ID3D11InputLayout> m_spFBXBinModelVertexLayout;
        ComPtr<ID3D11VertexShader> m_spGBufferFBXBinModelVertexShader;
        ComPtr<ID3D11PixelShader> m_spGBufferFBXBinModelPixelShader;

        ComPtr<ID3D11InputLayout> m_spFBXBinSkinnedModelVertexLayout;
        ComPtr<ID3D11VertexShader> m_spGBufferFBXBinSkinnedModelVertexShader;
        ComPtr<ID3D11PixelShader> m_spGBufferFBXBinSkinnedModelPixelShader;

        ComPtr<ID3D11InputLayout> m_spAlbedoModelVertexLayout;
        ComPtr<ID3D11VertexShader> m_spGBufferAlbedoModelVertexShader;
        ComPtr<ID3D11PixelShader> m_spGBufferAlbedoModelPixelShader;

        ComPtr<ID3D11InputLayout> m_spTexturedModelVertexLayout;
        ComPtr<ID3D11VertexShader> m_spGBufferTexturedModelVertexShader;
        ComPtr<ID3D11PixelShader> m_spGBufferTexturedModelPixelShader;

        // GBuffer Visualizer Shaders
        ComPtr<ID3D11VertexShader> m_spGBufferVisualizerVertexShader;
        ComPtr<ID3D11PixelShader> m_spGBufferVisualizerPixelShader;

        // Light Shaders
        ComPtr<ID3D11InputLayout> m_spLightVertexLayout;
        ComPtr<ID3D11VertexShader> m_spDirectionalLightVertexShader;
        ComPtr<ID3D11PixelShader> m_spDirectionalLightPixelShader;

        // Post Processing Shaders
        ComPtr<ID3D11VertexShader> m_spContourDetectionPass1VertexShader;
        ComPtr<ID3D11PixelShader> m_spContourDetectionPass1PixelShader;

        ComPtr<ID3D11InputLayout> m_spFullScreenQuadVertexLayout;
        ComPtr<ID3D11VertexShader> m_spContourDetectionPass2VertexShader;
        ComPtr<ID3D11PixelShader> m_spContourDetectionPass2PixelShader;

        // Sprite Layout and Shaders
        ComPtr<ID3D11InputLayout> m_spSpriteVertexLayout;
        ComPtr<ID3D11VertexShader> m_spSpriteVertexShader;
        ComPtr<ID3D11PixelShader> m_spSpritePixelShader;
        ComPtr<ID3D11SamplerState> m_spLinearSampler;
        ComPtr<ID3D11SamplerState> m_spPointSampler;
        ComPtr<ID3D11BlendState1> m_spBlendStateEnable;
        ComPtr<ID3D11BlendState1> m_spBlendStateDisable;

        ComPtr<ID3D11Buffer> m_spConstantBufferPerFrame;
        ComPtr<ID3D11Buffer> m_spConstantBufferPerObjectVS;
        ComPtr<ID3D11Buffer> m_spConstantBufferPerObjectPS;
        ComPtr<ID3D11Buffer> m_spConstantBufferPerSpriteObject;
        ComPtr<ID3D11Buffer> m_spConstantBufferGBufferUnpack;
        ComPtr<ID3D11Buffer> m_spVertexBuffer;
        ComPtr<ID3D11Buffer> m_spFullScreenQuadVertexBuffer;
        ComPtr<ID3D11Buffer> m_spFullScreenQuadIndexBuffer;
        ComPtr<ID3D11Buffer> m_spIndexBuffer;

        ComPtr<ID3D11Buffer> m_spQuadVertexBuffer;
        ComPtr<ID3D11Buffer> m_spQuadIndexBuffer;

        UINT m_FullScreenQuadStride;
        UINT m_FullScreenQuadOffset;

        XMFLOAT4X4  m_World;
        XMFLOAT4X4	m_View;
        XMFLOAT4X4	m_Projection;
        CD3D11_VIEWPORT m_Viewport;
        bool m_bInitialized;

        //ConstantBuffer m_CB;
        float m_Rotate;
        std::hash_map<std::string, AlbedoModel > m_AlbedoModelMap;
        std::hash_map<std::string, TexturedModel > m_TexturedModelMap;

        // D2D Resources	
        ComPtr<ID2D1Factory1> m_spD2DFactory;
        ComPtr<ID2D1Device> m_spD2DDevice;
        ComPtr<ID2D1DeviceContext> m_spD2DDeviceContext;
        ComPtr<ID2D1Bitmap1> m_spD2DTargetBitmap;
        ComPtr<ID2D1SolidColorBrush> m_spWhiteBrush;
        ComPtr<ID2D1SolidColorBrush> m_spRedBrush;
        ComPtr<ID2D1SolidColorBrush> m_spGreenBrush;
        ComPtr<ID2D1SolidColorBrush> m_spBlueBrush;
        ComPtr<ID2D1SolidColorBrush> m_spOrangeBrush;
        ComPtr<ID2D1SolidColorBrush> m_spYellowBrush;
        std::hash_map<std::string, ComPtr<ID3D11ShaderResourceView> > m_TextureMap;
        std::hash_map<std::string, ComPtr<ID2D1Bitmap1> > m_BitmapMap;

        // DWrite Resources
        ComPtr<IDWriteFactory1> m_spDWriteFactory;

        // WIC Resources
        ComPtr<IWICImagingFactory> m_spWICFactory;

        // Window Properties
        WindowSystem* m_Window;
        XMUINT2 m_WindowSize;
        float m_DPIX;
        float m_DPIY;

        int m_DrawDebug;
        int m_ChooseModel;

        // Camera
        Camera* m_pCurrentCamera;

        // Component Lists
        std::list<Sprite*> m_SpriteList;
        std::list<Sprite*> m_BackgroundSpriteList;
        std::list<Model*> m_ModelList;
        std::list<Camera*> m_CameraList;
        std::list<Text*> m_TextList;
        std::list<Image*> m_ImageList;
        std::list<float> FramesPerSecond;

        // CS 560 specific stuff
        void DrawPathMidpoint();
        void DrawPathSplineInterpolation();
        void DrawIKTarget();
        float m_PathPointRadius = 5.0f;
        bool m_bEditPath;
        XMFLOAT2 m_MousePos;
        std::vector<XMFLOAT3> GraphicsSystem::ExpandMidpointList(std::vector<XMFLOAT3> Midpoints);
        void GenerateSplineInterpolationSystem();
        std::vector<double> BuildAndSolveLinearSystem(std::vector<double> coordlist);
        std::vector<std::vector<double> > m_LinearSystem;
        std::vector<std::vector<double> > m_LinearSystemDoublePrimed;
        size_t m_MaximumDegree = 20;
        std::vector<double> m_XConst;
        std::vector<double> m_ZConst;
        double Spline(std::vector<double> constants, double t);
        XMFLOAT2 SplineInterpolate(double t);
        float m_GroundLevel = 0.0f;
        void BuildArcLengthTable();
        void NormalizeArcLengthTable();
        std::list<ArcLengthTableElement> m_ArcLengthTable;
        double m_ArcLength;
        double m_ArcLengthEpsilon = 0.01f;
        double m_SplineScale;
        double InverseArcLength(double len);
        double ArcLength(double t);
        std::list<ArcLengthTableElement>::iterator BinaryArcLookupByTime(double t, size_t begin, size_t end);
        std::list<ArcLengthTableElement>::iterator BinaryArcLookupByLength(double len, size_t begin, size_t end);
        float m_AnimationLength = 10.f;
        float m_NormalizedDistanceAlongArc;
        void UpdateAnimation(float dt);

        float m_coiDelta = 0.02f;
        float m_StepSizeFactor = 0.15f;
        bool m_bDoIK;
	};
	extern GraphicsSystem* g_GRAPHICSSYSTEM;
}
