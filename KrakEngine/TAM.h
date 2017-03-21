#pragma once
#include "Precompiled.h"
#include <vector>
#include <string>

namespace KrakEngine
{
    static float TAMStrokeMinHeight = .5f / 32.f;
    static float TAMStrokeMaxHeight = 2.f / 32.f;
    static float TAMStrokeMinWidth = 2.f / 32.f;
    static float TAMStrokeMaxWidth = 24.f / 32.f;
    float GetRandFloat01();
    typedef ComPtr<ID2D1Bitmap1> TAMTexture;
    struct TAMStroke
    {
        TAMStroke(int randomSeed)
        {
            // TAMTODO: find a better random distribution
            uvCoordinate.x = GetRandFloat01();
            uvCoordinate.y = GetRandFloat01();
            dimension.x = TAMStrokeMinWidth + (TAMStrokeMaxWidth - TAMStrokeMinWidth) * GetRandFloat01();
            dimension.y = TAMStrokeMinHeight + (TAMStrokeMaxHeight - TAMStrokeMinHeight) * GetRandFloat01();
        }
        XMFLOAT2 uvCoordinate;
        XMFLOAT2 dimension; //dimension as a proportion of the texture
        TAMTexture strokeTexture;
        friend  bool operator <(const TAMStroke& a, const TAMStroke& b)
        {
            return a.uvCoordinate.x + a.uvCoordinate.y < b.uvCoordinate.x + b.uvCoordinate.y;
        }
    };

    class TonalArtMapGenerator
    {
    public:
        TonalArtMapGenerator() {
            m_imageTable.resize(m_maxDimension);
            for (auto& row : m_imageTable)
            {
                row.resize(m_numberOfShades);
            }

            m_strokeTable.resize(m_maxDimension);
            for (auto& row : m_strokeTable)
            {
                row.resize(m_numberOfShades);
            }
        };
        TonalArtMapGenerator(int minMipResolution, int maxDimension, int numberOfShades)
            : m_minMipResolution(minMipResolution)
            , m_maxDimension(maxDimension)
            , m_numberOfShades(numberOfShades)
        {
            TonalArtMapGenerator();
        };
        void GenerateTAM(std::string filename);
        std::vector<std::vector<TAMTexture>> m_imageTable;
    private:
        void DrawStrokeToImage(TAMStroke stroke, TAMTexture image);
        TAMTexture CreateImageFromPastResults(int row, int column);
        std::vector<TAMStroke> GenerateRandomStrokes();
        TAMStroke PickBestStroke(std::vector<TAMStroke> randomStrokes, TAMTexture image, int column);
        void AddStrokeToImage(TAMStroke stroke, TAMTexture image);
        float GetToneForColumn(int column, int numberOfShades);
        float GetToneOfImage(TAMTexture image);
        float EvaluateImageValue(TAMStroke stroke, TAMTexture image, int column);
    
        std::vector<std::vector<std::vector<TAMStroke>>> m_strokeTable;

        TAMTexture m_strokeTexture;
        int m_maxDimension = 8; // power of 2
        int m_numberOfShades = 64; //number of columns to generate
        int m_minMipResolution = 32; // size of the smallest
        int m_numRandomStrokesPerIteration = 10;
        std::wstring m_imageFileName = L"C:/Users/Tommy/Documents/GitHub/KrakEngine/KrakEngine/Assets/Textures/pencilStroke1.png";
    };

}