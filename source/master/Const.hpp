#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class ConstData
{
public :
    int FPS;
    int window_width;
    int window_height;
    int backbuffer_width;
    int backbuffer_height;

    void Load(StreamReader& reader)
    {
        FPS = reader.ReadInt();
        window_width = reader.ReadInt();
        window_height = reader.ReadInt();
        backbuffer_width = reader.ReadInt();
        backbuffer_height = reader.ReadInt();
    }
};
}
