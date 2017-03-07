#pragma once

#include <string>
#include <vector>
#include <map>
#include "master/Const.hpp"
#include "master/ConstRasterScroll.hpp"
#include "master/KeyRepeatBase.hpp"
#include "master/OptionImageList.hpp"
#include "master/OptionUI.hpp"
#include "master/OptionInOut.hpp"
#include "master/OptionConst.hpp"
#include "master/TitleImageList.hpp"
#include "master/TitleUI.hpp"
#include "master/TitleInOut.hpp"

namespace MasterData
{
    extern ConstData Const;
    extern ConstRasterScrollData ConstRasterScroll;
    extern std::vector<KeyRepeatBaseData> KeyRepeatBase;
    extern std::map<std::string, OptionImageListData> OptionImageList;
    extern std::vector<OptionUIData> OptionUI;
    extern std::map<std::string, OptionInOutData> OptionInOut;
    extern OptionConstData OptionConst;
    extern std::map<std::string, TitleImageListData> TitleImageList;
    extern std::vector<TitleUIData> TitleUI;
    extern std::map<std::string, TitleInOutData> TitleInOut;


    void Reload(const std::string& path);
}
