#pragma once
#include "MathLib.h"
namespace KrakEngine
{
    enum class Technique : int
    {
        PrincipalCurvature,
        Luminance,
        MaxTechnique
    };

    enum class DebugDrawingMode : int
    {
        Default = 0,
        LuminanceBuffer,
        DirectionBuffer,
        DirectionBufferX,
        DirectionBufferY,
        DirectionBufferZ,
        XYDirectionBuffer,
        XYDirectionBufferX,
        XYDirectionBufferY,
        UniformDirection,
        EndCycle,
        GBufferNormal,
        GBufferDepth,
        Depth,
        ProjectedNormal,
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
        Technique m_technique = Technique::PrincipalCurvature;

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
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Default\r";
                break;
            case DebugDrawingMode::DirectionBuffer:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Direction Buffer\r";
                break;
            case DebugDrawingMode::DirectionBufferX:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Direction BufferX\r";
                break;
            case DebugDrawingMode::DirectionBufferY:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Direction BufferY\r";
                break;
            case DebugDrawingMode::DirectionBufferZ:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Direction BufferZ\r";
                break;
            case DebugDrawingMode::GBufferNormal:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Normal Buffer\r";
                break;
            case DebugDrawingMode::GBufferDepth:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Depth Buffer\r";
                break;
            case DebugDrawingMode::LuminanceBuffer:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Luminance Buffer\r";
                break;
            case DebugDrawingMode::UniformDirection:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": Uniform Direction\r";
                break;
            case DebugDrawingMode::XYDirectionBuffer:
                str = "Cycle Render Mode(g): "  + std::to_string((int)m_drawingMode) + ": XY Direction Buffer\r";
                break;
            case DebugDrawingMode::XYDirectionBufferX:
                str = "Cycle Render Mode(g): " + std::to_string((int)m_drawingMode) + ": XY Direction Buffer X\r";
                break;
            case DebugDrawingMode::XYDirectionBufferY:
                str = "Cycle Render Mode(g): " + std::to_string((int)m_drawingMode) + ": XY Direction Buffer Y\r";
                break;
            case DebugDrawingMode::AmbientOcclusion:
                str = "Cycle Render Mode(g): " + std::to_string((int)m_drawingMode) + ": Ambient Occlusion\r";
                break;

            default:
                break;
            }

            switch (m_technique)
            {
            case Technique::Luminance:
                str.append("Cycle Technique(t): Luminance\r");
                break;
            case Technique::PrincipalCurvature:
                str.append("Cycle Technique(t): Principal Curvature\r");
                break;
            default:
                break;
            }

            str.append("Toggle Blur(b): " + std::string(m_isGradientBufferBlurOn ? "On\r" : "Off\r"));
            str.append("Num Blur Passes(-c, +v): " + std::to_string(m_numBlurPasses) + "\r");
            return str;
        }

        void CycleDrawingMode()
        {
            m_drawingMode = static_cast<DebugDrawingMode>(((int)m_drawingMode + 1) % (int)DebugDrawingMode::EndCycle);
        }

        void CycleTechnique()
        {
            m_technique = static_cast<Technique>(((int)m_technique + 1) % (int)Technique::MaxTechnique);
        }
    };
}