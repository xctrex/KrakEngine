///////////////////////////////////////////////////////////////////////////////////////
//
//	ChunkReader.hpp
//	Simple binary chunk based file reader.
//
//	Authors: Chris Peters
//	Copyright 2009, Digipen Institute of Technology
//
///////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <fstream>
namespace KrakEngine{
    struct GChunk
    {
        UINT Type;
        UINT Size;
        UINT StartPos;
        UINT EndPos;
    };

    class ChunkReader
    {
    public:
        ChunkReader(){};

        void Open(std::string filename)
        {
            file.open(filename.c_str(), std::ios::binary | std::ios::in);
        }

        void Close()
        {
            file.close();
        }

        GChunk PeekChunk()
        {
            GChunk c = ReadChunkHeader();
            file.seekg(-int(sizeof(UINT) * 2), std::ios::cur);
            return c;
        }

        GChunk ReadChunkHeader()
        {
            GChunk c;
            c.Type = 0;
            file.read((char*)&c.Type, sizeof(UINT) * 2);
            c.StartPos = file.tellg();
            c.EndPos = c.StartPos + c.Size;
            return c;
        }

        GChunk ReadUntilChunk(UINT chunktype)
        {
            GChunk Chunk = ReadChunkHeader();
            while (Chunk.Type != chunktype && Chunk.Type != 0)
            {
                SkipChunk(Chunk);
                Chunk = ReadChunkHeader();
            }
            return Chunk;
        }

        void SkipChunk(GChunk& c)
        {
            file.seekg(c.EndPos);
        }

        void MoveToChunk(GChunk& c)
        {
            file.clear();
            file.seekg(c.StartPos);
        }

        template< typename type>
        void Read(type& data)
        {
            file.read((char*)&data, sizeof(type));
        }

        void Read(std::string& str)
        {
            byte strSize = 0;
            Read(strSize);
            str.resize(strSize);
            ReadArray(&str[0], strSize);
        }

        template< typename type>
        void ReadArray(type * data, int Number)
        {
            file.read((char*)data, sizeof(type) * Number);
        }

        template< typename type>
        void ReadArraySize(type * data, int Size)
        {
            file.read((char*)data, Size);
        }

    private:
        std::ifstream file;
    };
}
