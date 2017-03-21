#pragma once
#include "MathLib.h"
namespace KrakEngine
{
    enum class DebugDrawingMode : int
    {
        Default = 0,
        GBufferDepth,
        GBufferNormal,
        GBufferLuminance,
        UniformDirection,
        Depth,
        Luminance,
        ProjectedNormal,
        PrincipleCurvature,
        AmbientOcclusion,
        DistanceToFeatureLine,
        CombDirection,
        EdgeDetection,
        Skeleton
    };

    class DrawingState
    {
    public:
        DrawingState() {};
        ~DrawingState() {};

        DebugDrawingMode m_drawingMode = DebugDrawingMode::Default;

        bool m_isSkinningOn = false;

        bool m_isLightDynamic = false;
        float m_lightTime = 4.0f;
        float m_lightPosition[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

        bool m_isStrokeDirectionDynamic = false;
        float m_strokeDirectionTime = 0.0f;

        void Update(float dt)
        {
            // Update the lighting
            if (m_isLightDynamic)
            {
                float multiplier = 50.f;
                m_lightTime += dt;
                m_lightPosition[0] = sin(m_lightTime) * multiplier;
                m_lightPosition[1] = 10.f;
                m_lightPosition[2] = cos(m_lightTime) * multiplier;
                m_lightPosition[3] = .5f;
            }

            if (m_isStrokeDirectionDynamic)
            {
                m_strokeDirectionTime += dt;
            }
        }

        std::string GetDebugModeString()
        {
            std::string str = "";
            switch (m_drawingMode)
            {
            case DebugDrawingMode::Default:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Default";
                break;
            case DebugDrawingMode::GBufferNormal:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Normal Buffer";
                break;
            case DebugDrawingMode::GBufferDepth:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Depth Buffer";
                break;
            case DebugDrawingMode::GBufferLuminance:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Luminance Buffer";
                break;
            case DebugDrawingMode::UniformDirection:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Uniform Direction";
                break;

            default:
                break;
            }
            return str;
        }
    };
}