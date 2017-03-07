#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class ConstRasterScrollData
{
public :
    float offset_rate;
    float period_rate;
    float amplitude_rate;
    float offset;
    float period;
    float amplitude;
    float scroll_time;
    float fade_time;

    void Load(StreamReader& reader)
    {
        offset_rate = reader.ReadFloat();
        period_rate = reader.ReadFloat();
        amplitude_rate = reader.ReadFloat();
        offset = reader.ReadFloat();
        period = reader.ReadFloat();
        amplitude = reader.ReadFloat();
        scroll_time = reader.ReadFloat();
        fade_time = reader.ReadFloat();
    }
};
}
