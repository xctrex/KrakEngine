#pragma once
#include "MathLib.h"
namespace KrakEngine
{
    enum class DebugDrawingMode : int
    {
        Default = 0,
        LuminanceBuffer,
        DirectionBuffer,
        DirectionBufferX,
        DirectionBufferY,
        DirectionBufferZ,
        LuminanceGradientBuffer,
        LuminanceGradientBufferX,
        LuminanceGradientBufferY,
        LuminanceGradientBufferZ,
        UniformDirection,
        EndCycle,
        GBufferNormal,
        GBufferDepth,
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

        bool m_isGradientBufferBlurOn = false;
        int m_numBlurPasses = 1;

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
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Default\r";
                break;
            case DebugDrawingMode::DirectionBuffer:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Direction Buffer\r";
                break;
            case DebugDrawingMode::DirectionBufferX:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Direction BufferX\r";
                break;
            case DebugDrawingMode::DirectionBufferY:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Direction BufferY\r";
                break;
            case DebugDrawingMode::DirectionBufferZ:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Direction BufferZ\r";
                break;
            case DebugDrawingMode::GBufferNormal:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Normal Buffer\r";
                break;
            case DebugDrawingMode::GBufferDepth:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Depth Buffer\r";
                break;
            case DebugDrawingMode::LuminanceBuffer:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Luminance Buffer\r";
                break;
            case DebugDrawingMode::UniformDirection:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Uniform Direction\r";
                break;
            case DebugDrawingMode::LuminanceGradientBuffer:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Luminance Gradient Buffer\r";
                break;
            case DebugDrawingMode::LuminanceGradientBufferX:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Luminance Gradient Buffer X\r";
                break;
            case DebugDrawingMode::LuminanceGradientBufferY:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Luminance Gradient Buffer Y\r";
                break;
            case DebugDrawingMode::LuminanceGradientBufferZ:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Luminance Gradient Buffer Z\r";
                break;
            case DebugDrawingMode::AmbientOcclusion:
                str = "\rRender Mode " + std::to_string((int)m_drawingMode) + ": Ambient Occlusion\r";
                break;

            default:
                break;
            }
            str.append("\rToggle Blur(b): " + m_isGradientBufferBlurOn ? "On" : "Off");
            str.append("\rNum Blur Passes(-x, +c): " + std::to_string(m_numBlurPasses));
            return str;
        }

        void CycleDrawingMode()
        {
            m_drawingMode = static_cast<DebugDrawingMode>(((int)m_drawingMode + 1) % (int)DebugDrawingMode::EndCycle);
        }
    };
}