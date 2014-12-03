/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Header for Camera
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 1/27/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "Component.h"
#include "GraphicsSystem.h"

namespace KrakEngine{

    class Transform;

    class Camera : public Component{



    public:
	    Camera();
        ~Camera(){};
        
			enum PanStatus
	{
		No_Pan,
		Pan_Up,
		Pan_Down
	};

        virtual void Initialize() {};
        virtual void Shutdown() {};
		virtual void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);

        void Update(float dt);
	    XMFLOAT4X4* GetView();
        XMFLOAT3 GetLookatPosition();

        void MoveLeftRight(float movement){ m_MoveLeftRight = movement; }
        void MoveBackForward(float movement){ m_MoveBackForward = movement; }
        void MoveUpDown(float movement){ m_MoveUpDown = movement; }
        void AdjustPitch(float adjustment){ m_CamPitch += adjustment; }
        void AdjustYaw(float adjustment){ m_CamYaw += adjustment; }

        void SetYOffset(float y){ m_YOffset = y; }

		void SetDepth(float depth) 
		{ 
			std::stringstream ss;
			ss << "SetDepth(): " << depth << std::endl;
			OutputDebugStringA(ss.str().c_str());

			m_Depth = depth; 
		}
		void SetHeight(float height) { m_Height = height; }
		void SetLookHeight(float lookheight) { m_LookHeight = lookheight; }
		float GetDepth() 
		{ 
			std::stringstream ss;
			ss << "GetDepth(): " << m_Depth << std::endl;
			OutputDebugStringA(ss.str().c_str());
			return m_Depth; 
		}
		float GetHeight() { return m_Height; }
		float GetLookHeight() { return m_LookHeight; }
		
		void SetLookAtPosition(XMFLOAT3 position);

		void SetFreeCamera(bool enable) { m_freeCamera = enable; }
		bool IsFreeCameraSet() { return m_freeCamera; }

		float GetTargetHeight() { return m_targetHeight; }
		void SetTargetHeight(float height) { m_targetHeight = height; }

		float GetTargetLookHeight() { return m_targetLook; }
		void SetTargetLookHeight(float height) { m_targetLook = height; }

		void SetPan(PanStatus pan) { m_Pan = pan; }
		PanStatus GetPan() { return m_Pan; }

	private:
        XMFLOAT4X4  m_View;
        float m_Depth;
        float m_Height;
        float m_LookHeight;
        float m_YOffset;
        float m_Rotation;

		bool m_freeCamera;

		float m_targetHeight;
		float m_targetLook;

		PanStatus m_Pan;

        // Camera controls for free roam
        XMFLOAT4 m_DefaultForward;
        XMFLOAT4 m_DefaultRight;
        XMFLOAT4 m_CamForward;
        XMFLOAT4 m_CamRight;
        XMFLOAT4 m_CamUp;

        XMFLOAT4X4 m_CamRotationMatrix;
        XMFLOAT4X4 m_GroundWorld;

        float m_MoveLeftRight;
        float m_MoveBackForward;
        float m_MoveUpDown;

        float m_CamYaw;
        float m_CamPitch;

        XMFLOAT3 m_CamTarget;
        XMFLOAT4X4 m_CamView;
		XMFLOAT4X4 m_CamWorld;
        XMFLOAT3 m_CamPosition;
		XMFLOAT3 m_Velocity;
    };
}
