#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class OptionInOutData
{
public :
    std::string name;
    int type;
    float time;
    float in_delay;
    float out_delay;
    int param1;

    void Load(StreamReader& reader)
    {
        name = reader.ReadString();
        type = reader.ReadInt();
        time = reader.ReadFloat();
        in_delay = reader.ReadFloat();
        out_delay = reader.ReadFloat();
        param1 = reader.ReadInt();
    }
};
}
