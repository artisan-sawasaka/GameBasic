#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class OptionConstData
{
public :
    int CursorMove;
    int CursorSpeed;

    void Load(StreamReader& reader)
    {
        CursorMove = reader.ReadInt();
        CursorSpeed = reader.ReadInt();
    }
};
}
