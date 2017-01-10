#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class KeyRepeatBaseData
{
public :
    int id;
    int value;

    void Load(StreamReader& reader)
    {
        id = reader.ReadInt();
        value = reader.ReadInt();
    }
};
}
