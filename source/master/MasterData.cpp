#include "MasterData.hpp"
#include "utility/FileUtility.hpp"
#include "utility/StreamReader.hpp"
#include <functional>

namespace MasterData
{
    ConstData Const;
    ConstRasterScrollData ConstRasterScroll;
    OptionConstData OptionConst;
    std::map<std::string, OptionImageListData> OptionImageList;
    std::map<std::string, OptionInOutData> OptionInOut;
    std::map<std::string, TitleImageListData> TitleImageList;
    std::map<std::string, TitleInOutData> TitleInOut;
    std::vector<KeyRepeatBaseData> KeyRepeatBase;
    std::vector<OptionUIData> OptionUI;
    std::vector<TitleUIData> TitleUI;

    void LoadConst(const std::string& path, std::function<void(StreamReader&)> func) {
        auto buffer = File::ReadAllBytes(path);
        auto reader = StreamReader(buffer);
        func(reader);
    }

    template <class T>
    void LoadArray(const std::string& path, T& value) {
        auto buffer = File::ReadAllBytes(path);
        auto reader = StreamReader(buffer);
        T().swap(value);
        value.resize(reader.ReadInt());
        for (size_t i = 0; i < value.size(); ++i) {
            value[i].Load(reader);
        }
    }

    template <class T>
    void LoadMap(const std::string& path, T& value) {
        auto buffer = File::ReadAllBytes(path);
        auto reader = StreamReader(buffer);
        auto length = reader.ReadInt();
        value.clear();
        for (int i = 0; i < length; ++i) {
            auto key = reader.ReadStringNoSeek();
            value[key].Load(reader);
        }
    }

    void Reload(const std::string& path) {
        LoadArray(path + "/KeyRepeatBase.dat", KeyRepeatBase);
        LoadArray(path + "/OptionUI.dat", OptionUI);
        LoadArray(path + "/TitleUI.dat", TitleUI);
        LoadConst(path + "/Const.dat", std::bind(&ConstData::Load, &Const, std::placeholders::_1));
        LoadConst(path + "/ConstRasterScroll.dat", std::bind(&ConstRasterScrollData::Load, &ConstRasterScroll, std::placeholders::_1));
        LoadConst(path + "/OptionConst.dat", std::bind(&OptionConstData::Load, &OptionConst, std::placeholders::_1));
        LoadMap(path + "/OptionImageList.dat", OptionImageList);
        LoadMap(path + "/OptionInOut.dat", OptionInOut);
        LoadMap(path + "/TitleImageList.dat", TitleImageList);
        LoadMap(path + "/TitleInOut.dat", TitleInOut);
    }
}
