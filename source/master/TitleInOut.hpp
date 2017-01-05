#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class TitleInOutData
{
public :
    std::string name;
    int type;
    float time;
    float delay;
    int param1;
    int param2;

    void Load(StreamReader& reader)
    {
        name = reader.ReadString();
        type = reader.ReadInt();
        time = reader.ReadFloat();
        delay = reader.ReadFloat();
        param1 = reader.ReadInt();
        param2 = reader.ReadInt();
    }
};
}
