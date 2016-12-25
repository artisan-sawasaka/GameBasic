#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class TitleImageListData
{
public :
    std::string name;
    std::string path;
    int x;
    int y;
    int w;
    int h;

    void Load(StreamReader& reader)
    {
        name = reader.ReadString();
        path = reader.ReadString();
        x = reader.ReadInt();
        y = reader.ReadInt();
        w = reader.ReadInt();
        h = reader.ReadInt();
    }
};
}
