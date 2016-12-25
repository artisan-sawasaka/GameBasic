#pragma once

#include <string>
#include "utility/StreamReader.hpp"

namespace MasterData
{
class TitleUIData
{
public :
    int id;
    std::string name;
    int type;
    std::string str;
    int anchor;
    int x;
    int y;
    int w;
    int h;
    int r;
    int g;
    int b;
    int a;

    void Load(StreamReader& reader)
    {
        id = reader.ReadInt();
        name = reader.ReadString();
        type = reader.ReadInt();
        str = reader.ReadString();
        anchor = reader.ReadInt();
        x = reader.ReadInt();
        y = reader.ReadInt();
        w = reader.ReadInt();
        h = reader.ReadInt();
        r = reader.ReadInt();
        g = reader.ReadInt();
        b = reader.ReadInt();
        a = reader.ReadInt();
    }
};
}
