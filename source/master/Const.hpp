#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class ConstData
{
public :
    int FPS;
    float x1;
    float y1;
    float x2;
    float y2;

    void Load(StreamReader& reader)
    {
        FPS = reader.ReadInt();
        x1 = reader.ReadFloat();
        y1 = reader.ReadFloat();
        x2 = reader.ReadFloat();
        y2 = reader.ReadFloat();
    }
};
}
