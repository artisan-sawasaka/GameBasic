#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class ConstData
{
public :
    int FPS;

    void Load(StreamReader& reader)
    {
        FPS = reader.ReadInt();
    }
};
}
