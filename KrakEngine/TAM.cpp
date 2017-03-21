#include "Precompiled.h"
#include "TAM.h"
#include "GraphicsSystem.h"
#include "C:\Users\Tommy\Downloads\DirectXTex-master\DirectXTex-master\DirectXTex\DirectXTex.h"

namespace KrakEngine
{
    float GetRandFloat01()
    {
        return (float)(rand() % 100000) / 100000.f;
    }

    void TonalArtMapGenerator::GenerateTAM(std::string filename)
    {
        m_strokeTexture = g_GRAPHICSSYSTEM->CreateBitmapFromFile(m_imageFileName);

        m_imageTable.resize(m_maxDimension + 1);
        m_strokeTable.resize(m_maxDimension + 1);
        for (int column = 0; column < m_numberOfShades; ++column)
        {
            for (int row = 0; row <= m_maxDimension; ++row)
            {
                m_imageTable[row].resize(m_numberOfShades);
                m_strokeTable[row].resize(m_numberOfShades);

                float targetTone = GetToneForColumn(column, m_numberOfShades);
                TAMTexture image = CreateImageFromPastResults(row, column);
                // Create a readable copy of the image
                TAMTexture readImage = g_GRAPHICSSYSTEM->CreateReadableBitmapCopy(image);
                float currentTone = GetToneOfImage(readImage);
                while (currentTone < targetTone)
                {
                    std::vector<TAMStroke> randomStrokes = GenerateRandomStrokes();
                    for (TAMStroke& stroke : randomStrokes)
                    {
                        stroke.strokeTexture = m_strokeTexture;
                    }
                    TAMStroke bestStroke = PickBestStroke(randomStrokes, image, column);
                    DrawStrokeToImage(bestStroke, image);
                    m_strokeTable[row][column].push_back(bestStroke);

                    readImage = g_GRAPHICSSYSTEM->CreateReadableBitmapCopy(image);
                    currentTone = GetToneOfImage(readImage);
                }

                m_imageTable[row][column] = g_GRAPHICSSYSTEM->CreateBitmapCopy(readImage, D2D1_BITMAP_OPTIONS_NONE);
                
                g_GRAPHICSSYSTEM->SaveBitmapToFile(m_imageTable[row][column], GUID_ContainerFormatPng, L"crosshatch" + std::to_wstring(row) + L"_" + std::to_wstring(column) + L".png");
            }
        }
    }

    void TonalArtMapGenerator::DrawStrokeToImage(TAMStroke stroke, TAMTexture image)
    {
        g_GRAPHICSSYSTEM->Set2DTarget(image);
        g_GRAPHICSSYSTEM->DrawTAMStroke(stroke, image);
    }

    TAMTexture TonalArtMapGenerator::CreateImageFromPastResults(int row, int column)
    {
        std::vector<TAMStroke> strokes;
        if (column > 0)
        {
            strokes = m_strokeTable[row][column - 1];
        }
        if (row > 0)
        {
            for (TAMStroke stroke : m_strokeTable[row - 1][column])
            {
                strokes.push_back(stroke);
            }
        }
        //Some strokes in the current column, previous row are new, but some existed in the current row previous column
        // Remove duplicates
        std::set<TAMStroke> uniqueStrokes(strokes.begin(), strokes.end());
        strokes.assign(uniqueStrokes.begin(), uniqueStrokes.end());

        //Allocate new image based on dimensions
        int dimensions = m_minMipResolution * pow(2, row);

        // TAMTODO: investigate if anyone has tried using multiple stroke images or procedurally generated stroke images
        // TAMTODO: use dimension
        TAMTexture image = g_GRAPHICSSYSTEM->CreateWriteableBitmap(D2D1::SizeU(dimensions, dimensions));//g_GRAPHICSSYSTEM->CreateBitmapFromFile(m_imageFileName, D2D1_BITMAP_OPTIONS_TARGET);

        //TODO: get already created image instead of re-creating it

        // Draw the existing strokes
        g_GRAPHICSSYSTEM->Set2DTarget(image);
        g_GRAPHICSSYSTEM->Begin2DDraw();
        g_GRAPHICSSYSTEM->Clear2DRenderTarget(D2D1::ColorF(D2D1::ColorF::White));
        g_GRAPHICSSYSTEM->End2DDraw();
        for (auto stroke : strokes)
        {
            g_GRAPHICSSYSTEM->DrawTAMStroke(stroke, image);
        }

        return image;
    }

    std::vector<TAMStroke> TonalArtMapGenerator::GenerateRandomStrokes()
    {
        std::vector<TAMStroke> randomStrokes;
        for (int i = 0; i < m_numRandomStrokesPerIteration; ++i)
        {
            randomStrokes.push_back(TAMStroke(rand()));
        }

        return randomStrokes;
    }

    TAMStroke TonalArtMapGenerator::PickBestStroke(std::vector<TAMStroke> randomStrokes, TAMTexture image, int column)
    {
        TAMStroke bestFitStroke = 0;
        float bestFitValue = 0;
        for (auto stroke : randomStrokes)
        {
            TAMTexture tempImage = g_GRAPHICSSYSTEM->CreateBitmapCopy(image);

            // Add stroke to image
            g_GRAPHICSSYSTEM->Set2DTarget(tempImage);
            g_GRAPHICSSYSTEM->DrawTAMStroke(stroke, tempImage);
            float value = EvaluateImageValue(stroke, tempImage, column);
            if (value > bestFitValue)
            {
                bestFitValue = value;
                bestFitStroke = stroke;
            }
        }

        return bestFitStroke;
    }

    float TonalArtMapGenerator::GetToneForColumn(int column, int numberOfShades)
    {
        return (float)column / (float)numberOfShades;
    }

    float TonalArtMapGenerator::GetToneOfImage(TAMTexture image)
    {
        float totalColor = 0.f;
        int numPixels = image.Get()->GetPixelSize().height * image.Get()->GetPixelSize().width;
        D2D1_MAP_OPTIONS options = D2D1_MAP_OPTIONS_READ;
        D2D1_MAPPED_RECT mappedRect;
        DXThrowIfFailed(image.Get()->Map(options, &mappedRect));
        for (int i = 0; i <numPixels; ++i)
        {
            totalColor += 255.f - mappedRect.bits[i];
            //TAMTODO: currently only not adding per-channel
        }
        totalColor /= (float)numPixels * 255.f;
        image.Get()->Unmap();
        return totalColor;
    }

    float TonalArtMapGenerator::EvaluateImageValue(TAMStroke stroke, TAMTexture image, int column)
    {
        // TAMTODO: generate a rating for the stroke
        return (float)(rand() % 100);
    }
}