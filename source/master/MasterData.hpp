#pragma once

#include <string>
#include <vector>
#include <map>
#include "master/TitleImageList.hpp"
#include "master/TitleUI.hpp"
#include "master/Const.hpp"

namespace MasterData
{
    extern std::map<std::string, TitleImageListData> TitleImageList;
    extern std::vector<TitleUIData> TitleUI;
    extern ConstData Const;


    void Reload(const std::string& path);
}
