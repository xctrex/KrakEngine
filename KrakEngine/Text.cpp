/* Start Header -------------------------------------------------------
Copyright (C) 2013 DigiPen Institute of Technology. Reproduction or disclosure of this file or its contents without the prior written consent of DigiPen Institute of Technology is prohibited.
File Name: Text.cpp
Purpose: Component for Text
Language: C++, MSC
Platform: Windows
Project: go_ninja
Author: Tommy Walton
Creation date: 1/27/2014
- End Header -----------------------------------------------------*/

#include "Text.h"
#include "Transform.h"
#include "GameObject.h"

namespace KrakEngine{

    Text::Text() :
        Component(TextComp),
        m_Position(0.05f, 0.05f, 10.0f),
        m_TextContent("Default"),
        //m_Font("Segoe UI")
		m_Font("Hand Me Down S (BRK)")
    {
        g_GRAPHICSSYSTEM->AddTextToList(this);
    }

    void Text::SerializeViaXML(Serializer& stream){        
		InitializeDataMember(stream, m_Position.x, "PosX");
		InitializeDataMember(stream, m_Position.y, "PosY");
		InitializeDataMember(stream, m_Position.z, "PosZ");
		InitializeDataMember(stream, m_TextContent, "Content");
    }

	void Text::SerializeToXML(tinyxml2::XMLElement * element)
	{
		WriteDataMember(element, m_Position.x, "PosX");
		WriteDataMember(element, m_Position.y, "PosY");
		WriteDataMember(element, m_Position.z, "PosZ");
		WriteDataMember(element, m_TextContent, "Content");
    }

    void Text::Draw(const ComPtr<ID2D1DeviceContext> &spD2DDeviceContext, const ComPtr<ID2D1SolidColorBrush> &spBrush, const ComPtr<IDWriteFactory1> &spDWriteFactory){
        // Convert the text content and the font to wide strings
        wchar_t content[512];
        swprintf(content, m_TextContent.length() + 1, L"%hs", m_TextContent.c_str());
        wchar_t font[512];
        swprintf(font, m_Font.length() + 1, L"%hs", m_Font.c_str());

        m_Rect.left = m_Position.x * g_GRAPHICSSYSTEM->GetWindowSize().x;
        m_Rect.top = m_Position.y * g_GRAPHICSSYSTEM->GetWindowSize().y;
        m_Rect.right = m_Rect.left + 512.0f;
        m_Rect.bottom = m_Rect.top + 512.0f;

        ComPtr<IDWriteTextFormat> spDWriteTextFormat;
        DXThrowIfFailed(
            spDWriteFactory->CreateTextFormat(
                font,
                NULL,
                DWRITE_FONT_WEIGHT_REGULAR,
                DWRITE_FONT_STYLE_NORMAL,
                DWRITE_FONT_STRETCH_NORMAL,
                24.0f,
                L"en-us",
                &spDWriteTextFormat
            )
        );

        spD2DDeviceContext->DrawTextA(
            content,
            (UINT32)m_TextContent.length(),
            spDWriteTextFormat.Get(),
            m_Rect,
            spBrush.Get()
            );
    }
}
