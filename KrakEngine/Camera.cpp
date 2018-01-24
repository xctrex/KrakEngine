/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Component for Camera
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton
Creation date: 1/27/2014
- End Header -----------------------------------------------------*/

#include "Camera.h"
#include "Transform.h"
#include "GameObject.h"

namespace KrakEngine{

    Camera::Camera() :
        Component(CameraComp),
        m_View( 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f),
        m_Depth(-1.0f),
        m_Height(0.0f),
        m_LookHeight(0.0f),
        m_YOffset(0.0f),
        m_Rotation(0.0f),
		m_freeCamera(true),
		m_Pan(No_Pan),
        m_MoveLeftRight(0.0f),
        m_MoveBackForward(0.0f),
        m_MoveUpDown(0.0f),
        m_CamYaw(0.0f),
        m_CamPitch(0.0f),
        m_CamForward(0.0f, 0.0f, 1.0f, 0.0f),
        m_CamRight(1.0f, 0.0f, 0.0f, 0.0f),
        m_CamUp(0.0f, 1.0f, 0.0f, 0.0f),
        m_DefaultForward(0.0f, 0.0f, 1.0f, 0.0f),
        m_DefaultRight(1.0f, 0.0f, 0.0f, 0.0f),
        m_CamPosition(0.0f, 20.0f, -50.0f),
		m_CamTarget(0.0f, 0.0f, 0.0f),
		m_Velocity(1.0f, 1.0f, 1.0f)
    {
        XMStoreFloat4x4(&m_View, XMMatrixIdentity());
        g_GRAPHICSSYSTEM->AddCameraToList(this);
    }

    void Camera::SerializeViaXML(Serializer& stream){        
		InitializeDataMember(stream, m_Depth, "CameraDepth");
		
		std::stringstream ss;
		ss << "XML SerialDepth: " << m_Depth << std::endl;
		OutputDebugStringA(ss.str().c_str());

		InitializeDataMember(stream, m_Height, "CameraHeight");
		InitializeDataMember(stream, m_LookHeight, "LookHeight");
        InitializeDataMember(stream, m_YOffset, "YOffset");
		InitializeDataMember(stream, m_freeCamera, "Free");
    }
			
	void Camera::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_Depth, "CameraDepth");
		WriteDataMember(element, m_Height, "CameraHeight");
		WriteDataMember(element, m_LookHeight, "LookHeight");
        WriteDataMember(element, m_YOffset, "YOffset");
		WriteDataMember(element, m_freeCamera, "Free");
	}

	void Camera::Update(float dt){
        if(GetOwner()){
            // Set the x position and of the camera and target to the x coordinate of the parent object, and
            // set the height of the camera, depth of the camera, and height of the target based on the values specified in the xml
            XMVECTOR Eye;
            XMVECTOR LookAt;

            {
                // Calculate camera positions for free camera movement mode

                // Set the camera's target
                XMStoreFloat4x4(&m_CamRotationMatrix, XMMatrixRotationRollPitchYaw(m_CamPitch, m_CamYaw, 0.0f));
                XMStoreFloat3(&m_CamTarget, XMVector3TransformCoord(XMLoadFloat4(&m_DefaultForward), XMLoadFloat4x4(&m_CamRotationMatrix)));
                XMStoreFloat3(&m_CamTarget, XMVector3Normalize(XMLoadFloat3(&m_CamTarget)));

                // Find the new right and forward directions for the camera
                XMFLOAT4X4 RotateYTempMatrix;
                XMStoreFloat4x4(&RotateYTempMatrix, XMMatrixRotationY(m_CamPitch));

                // Update right, up, and forward vectors for the camera
                XMStoreFloat4(&m_CamRight, XMVector3TransformCoord(XMLoadFloat4(&m_DefaultRight), XMLoadFloat4x4(&RotateYTempMatrix)));
                XMStoreFloat4(&m_CamUp, XMVector3TransformCoord(XMLoadFloat4(&m_CamUp), XMLoadFloat4x4(&RotateYTempMatrix)));
                XMStoreFloat4(&m_CamForward, XMVector3TransformCoord(XMLoadFloat4(&m_DefaultForward), XMLoadFloat4x4(&RotateYTempMatrix)));

                // Move the camera
                XMFLOAT4 movement;
                XMStoreFloat4(&movement, XMVectorScale(XMLoadFloat4(&m_CamRight), m_MoveLeftRight));
                m_CamPosition.x += movement.x;
                m_CamPosition.y += movement.y;
                m_CamPosition.z += movement.z;

                XMStoreFloat4(&movement, XMVectorScale(XMLoadFloat4(&m_CamForward), m_MoveBackForward));
                m_CamPosition.x += movement.x;
                m_CamPosition.y += movement.y;
                m_CamPosition.z += movement.z;

                XMStoreFloat4(&movement, XMVectorScale(XMLoadFloat4(&m_CamUp), m_MoveUpDown));
                m_CamPosition.x += movement.x;
                m_CamPosition.y += movement.y;
                m_CamPosition.z += movement.z;

                // Reset movement variables for next frame
                m_MoveLeftRight = 0.0f;
                m_MoveBackForward = 0.0f;
                m_MoveUpDown = 0.0f;

                // Set the camera view matrix
                m_CamTarget.x = m_CamPosition.x + m_CamTarget.x;
                m_CamTarget.y = m_CamPosition.y + m_CamTarget.y;
                m_CamTarget.z = m_CamPosition.z + m_CamTarget.z;

                XMStoreFloat4x4(&m_CamView, XMMatrixLookAtLH(XMLoadFloat3(&m_CamPosition), XMLoadFloat3(&m_CamTarget), XMLoadFloat4(&m_CamUp)));
                m_View = m_CamView;

                // Simple euler method to calculate position delta
                XMVECTOR xmvVelocity = XMLoadFloat3(&XMFLOAT3(m_MoveLeftRight, m_MoveUpDown, m_MoveBackForward));
                XMVECTOR xmvPosDelta = xmvVelocity * dt;

                // Make a rotation matrix based on the camera's yaw & pitch
                XMMATRIX xmmCameraRot = XMMatrixRotationRollPitchYaw(m_CamPitch, m_CamYaw, 0.0f);

                // Transform vectors based on camera's rotation matrix
                XMVECTOR xmvWorldUp = XMVector3TransformCoord(g_XMIdentityR1, xmmCameraRot);
                XMVECTOR xmvWorldAhead = XMVector3TransformCoord(g_XMIdentityR2, xmmCameraRot);

                // Transform the position delta by the camera's rotation 
                /*if (!m_bEnableYAxisMovement)
                {
                    // If restricting Y movement, do not include pitch
                    // when transforming position delta vector.
                    mCameraRot = XMMatrixRotationRollPitchYaw(0.0f, m_fCameraYawAngle, 0.0f);
                }*/
                XMVECTOR xmvPosDeltaWorld = XMVector3TransformCoord(xmvPosDelta, xmmCameraRot);

                // Move the eye position 
                XMVECTOR xmvEye = XMLoadFloat3(&m_CamPosition);
                xmvEye += xmvPosDeltaWorld;
                /*if (m_bClipToBoundary)
                    vEye = ConstrainToBoundary(vEye);*/
                XMStoreFloat3(&m_CamPosition, xmvEye);

                // Update the lookAt position based on the eye position
                XMVECTOR xmvLookAt = xmvEye + xmvWorldAhead;
                XMStoreFloat3(&m_CamTarget, xmvLookAt);

                // Update the view matrix
                XMMATRIX xmmView = XMMatrixLookAtLH(xmvEye, xmvLookAt, xmvWorldUp);
                XMStoreFloat4x4(&m_View, xmmView);

                XMMATRIX mCameraWorld = XMMatrixInverse(nullptr, xmmView);
                XMStoreFloat4x4(&m_CamWorld, mCameraWorld);
            }
        }
    }
	
    XMFLOAT4X4* Camera::GetView(){
	    return &m_View;
    }

    XMFLOAT3 Camera::GetLookatPosition(){
        // Get the position of the owner
        Transform* t = GetOwner()->has(Transform);
        return  t->GetPosition();
    }

	void Camera::SetLookAtPosition(XMFLOAT3 position)
	{
		Transform* t = GetOwner()->has(Transform);
		t->SetPosition(position);
	}
}
