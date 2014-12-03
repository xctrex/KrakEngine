/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Text.h
Purpose: Header for Text
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

    class Text : public Component{
    public:
	    Text();
        ~Text(){};
        
        virtual void Initialize() {};
        virtual void Shutdown() {};
		virtual void SerializeViaXML(Serializer& stream);
		virtual void SerializeToXML(tinyxml2::XMLElement * element);
		
        void Draw(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1SolidColorBrush> &spBrush, const ComPtr<IDWriteFactory1> &spDWriteFactory);

        void SetTextContent(std::string Content){m_TextContent = Content;}
        
        XMFLOAT3 GetPosition() const{ return m_Position; }
        void SetPosition(XMFLOAT3 pos){ m_Position = pos; }

    private:
        std::string m_TextContent;
        XMFLOAT3 m_Position;
        std::string m_Font;
        D2D_RECT_F m_Rect;
    };
}
