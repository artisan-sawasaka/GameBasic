#pragma once

#include <string>
#include <vector>
#include <map>
#include "master/Const.hpp"
#include "master/TitleImageList.hpp"
#include "master/TitleUI.hpp"
#include "master/TitleInOut.hpp"

namespace MasterData
{
    extern ConstData Const;
    extern std::map<std::string, TitleImageListData> TitleImageList;
    extern std::vector<TitleUIData> TitleUI;
    extern std::map<std::string, TitleInOutData> TitleInOut;


    void Reload(const std::string& path);
}
