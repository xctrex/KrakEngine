/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Text.h
Purpose: Header for Screen Space Images
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton, t.walton
Creation date: 3/5/2014
- End Header -----------------------------------------------------*/

#pragma once
#include "Precompiled.h"
#include "Component.h"
#include "GraphicsSystem.h"

namespace KrakEngine{
    
    class Image : public Component{
    public:
	    Image();
        ~Image(){};
        
        virtual void Initialize() {};
        virtual void Shutdown() {};
		virtual void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);
		
        void Update(float dt);
        void UpdateAnimation(float dt, SpriteAnimation animation);
        void Draw(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext) const;

        XMFLOAT3 GetPosition() const{ return m_Position; }
        void SetPosition(XMFLOAT3 pos){ m_Position = pos; }
        
        XMFLOAT2 GetSize() const{ return m_Size; }
        void SetSize(XMFLOAT2 size){ m_Size = size; }
		void SetSize(float x, float y) {m_Size.x = x, m_Size.y = y;}

		void SetImageName(std::string name) {m_ImageName = name;}

    private:
        std::string m_ImageName;
        XMFLOAT3 m_Position;
        XMFLOAT2 m_Size;

        // Animation Data
        float m_FrameHeight;
        float m_FrameWidth;
        float m_FrameOffsetX;
        float m_FrameOffsetY;
        float m_Time;
        SpriteAnimation m_Animation;
        bool m_Animated;
    };
}
