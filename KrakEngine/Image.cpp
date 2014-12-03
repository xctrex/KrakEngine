/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Image.cpp
Purpose: Component for Screen Space Images
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton
Creation date: 1/27/2014
- End Header -----------------------------------------------------*/

#include "Image.h"
#include "Transform.h"
#include "GameObject.h"

namespace KrakEngine{

    Image::Image() :
        Component(ImageComp),
        m_Position(0.5f, 0.5f, 0.0f),
        m_Size(10.0f, 10.0f),
        m_ImageName("Default"),
        m_FrameHeight(0.0f),
        m_FrameWidth(0.0f),
        m_FrameOffsetX(0.0f),
        m_FrameOffsetY(0.0f),
        m_Time(0.0f),
        m_Animated(false)
    {
        g_GRAPHICSSYSTEM->AddImageToList(this);
    }

    void Image::SerializeViaXML(Serializer& stream){        
		InitializeDataMember(stream, m_Position.x, "PosX");
		InitializeDataMember(stream, m_Position.y, "PosY");
		InitializeDataMember(stream, m_Position.z, "PosZ");
        InitializeDataMember(stream, m_Size.x, "Width");
		InitializeDataMember(stream, m_Size.y, "Height");
        InitializeDataMember(stream, m_ImageName, "Image");
                
        InitializeDataMember(stream, m_Animation.Length, "AnimationLength");
        InitializeDataMember(stream, m_Animation.NumFrames, "AnimationNumFrames");
        InitializeDataMember(stream, m_Animation.StartFrame, "AnimationStartFrame");
        InitializeDataMember(stream, m_FrameWidth, "FrameWidth");
        InitializeDataMember(stream, m_FrameHeight, "FrameHeight");
        InitializeDataMember(stream, m_Animated, "Animated");
    }

	void Image::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_Position.x, "PosX");
		WriteDataMember(element, m_Position.y, "PosY");
		WriteDataMember(element, m_Position.z, "PosZ");
        WriteDataMember(element, m_Size.x, "Width");
		WriteDataMember(element, m_Size.y, "Height");
        WriteDataMember(element, m_ImageName, "Image");
                
        WriteDataMember(element, m_Animation.Length, "AnimationLength");
        WriteDataMember(element, m_Animation.NumFrames, "AnimationNumFrames");
        WriteDataMember(element, m_Animation.StartFrame, "AnimationStartFrame");
        WriteDataMember(element, m_FrameWidth, "FrameWidth");
        WriteDataMember(element, m_FrameHeight, "FrameHeight");
        WriteDataMember(element, m_Animated, "Animated");
    }

    void Image::Update(float dt){
        UpdateAnimation(dt, m_Animation);
    }

    void Image::UpdateAnimation(float dt, SpriteAnimation animation){
        if(m_Animated)
        {
            // Update the time
            m_Time += dt;
            // Loop back to the beginning of the animation if the time has run past the animation length
            if(m_Time > animation.Length)
                m_Time -= animation.Length;

            // Verify there is a valid number of frames
            ThrowErrorIf(animation.NumFrames <= 0.0f, "The animation must have a non-zero, positive number of frames");
            
            float timePerFrame = animation.Length / animation.NumFrames;
            UINT numxframes = (UINT)(g_GRAPHICSSYSTEM->GetBitmap(m_ImageName)->GetPixelSize().width / m_FrameWidth);
            UINT currentFrame = (UINT)(m_Time / timePerFrame) + animation.StartFrame;

            UINT currentXFrame = currentFrame % numxframes;
            UINT currentYFrame = currentFrame / numxframes;

            m_FrameOffsetX = m_FrameWidth * currentXFrame;
            m_FrameOffsetY = m_FrameHeight * currentYFrame;
        }
    }

    void Image::Draw(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const{
            
        if(g_GRAPHICSSYSTEM->GetBitmap(m_ImageName) != NULL){
            if(!m_Animated){
                spD2DDeviceContext->DrawBitmap(
                    g_GRAPHICSSYSTEM->GetBitmap(m_ImageName).Get(),
                    D2D1::RectF(
                        m_Position.x * g_GRAPHICSSYSTEM->GetWindowSize().x - m_Size.x * 0.5f,
                        m_Position.y * g_GRAPHICSSYSTEM->GetWindowSize().y - m_Size.y * 0.5f,
                        m_Position.x * g_GRAPHICSSYSTEM->GetWindowSize().x + m_Size.x * 0.5f,
                        m_Position.y * g_GRAPHICSSYSTEM->GetWindowSize().y + m_Size.y * 0.5f)
                    );
            }
            else{
                spD2DDeviceContext->DrawBitmap(
                    g_GRAPHICSSYSTEM->GetBitmap(m_ImageName).Get(),
                    D2D1::RectF(
                        m_Position.x * g_GRAPHICSSYSTEM->GetWindowSize().x - m_Size.x * 0.5f,
                        m_Position.y * g_GRAPHICSSYSTEM->GetWindowSize().y - m_Size.y * 0.5f,
                        m_Position.x * g_GRAPHICSSYSTEM->GetWindowSize().x + m_Size.x * 0.5f,
                        m_Position.y * g_GRAPHICSSYSTEM->GetWindowSize().y + m_Size.y * 0.5f),
                    1.0f,
                    D2D1_BITMAP_INTERPOLATION_MODE_LINEAR,
                    D2D1::RectF(
                        m_FrameOffsetX,
                        m_FrameOffsetY,
                        m_FrameOffsetX + m_FrameWidth,
                        m_FrameOffsetY + m_FrameHeight
                        )
                    );
            }
        }
        else{
            ThrowErrorIf(true, "m_ImageName not found in Bitmap map");
        }
    }
}
