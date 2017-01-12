#include "MasterData.hpp"
#include "utility/FileUtility.hpp"
#include "utility/StreamReader.hpp"

namespace MasterData
{
    ConstData Const;
    std::vector<KeyRepeatBaseData> KeyRepeatBase;
    std::map<std::string, OptionImageListData> OptionImageList;
    std::vector<OptionUIData> OptionUI;
    std::map<std::string, OptionInOutData> OptionInOut;
    OptionConstData OptionConst;
    std::map<std::string, TitleImageListData> TitleImageList;
    std::vector<TitleUIData> TitleUI;
    std::map<std::string, TitleInOutData> TitleInOut;


    void Reload(const std::string& path)
    {
        std::vector<char> buffer;
	    StreamReader reader;
        int length;

        buffer = File::ReadAllBytes(path + "/Const.dat");
        reader = StreamReader(buffer);
        Const.Load(reader);

        buffer = File::ReadAllBytes(path + "/KeyRepeatBase.dat");
        reader = StreamReader(buffer);
        KeyRepeatBase.resize(reader.ReadInt());
        for (size_t i = 0; i < KeyRepeatBase.size(); ++i) {
            KeyRepeatBase[i].Load(reader);
        }

        buffer = File::ReadAllBytes(path + "/OptionImageList.dat");
        reader = StreamReader(buffer);
        length = reader.ReadInt();
        for (int i = 0; i < length; ++i) {
            auto key = reader.ReadStringNoSeek();
            OptionImageList[key].Load(reader);
        }

        buffer = File::ReadAllBytes(path + "/OptionUI.dat");
        reader = StreamReader(buffer);
        OptionUI.resize(reader.ReadInt());
        for (size_t i = 0; i < OptionUI.size(); ++i) {
            OptionUI[i].Load(reader);
        }

        buffer = File::ReadAllBytes(path + "/OptionInOut.dat");
        reader = StreamReader(buffer);
        length = reader.ReadInt();
        for (int i = 0; i < length; ++i) {
            auto key = reader.ReadStringNoSeek();
            OptionInOut[key].Load(reader);
        }

        buffer = File::ReadAllBytes(path + "/OptionConst.dat");
        reader = StreamReader(buffer);
        OptionConst.Load(reader);

        buffer = File::ReadAllBytes(path + "/TitleImageList.dat");
        reader = StreamReader(buffer);
        length = reader.ReadInt();
        for (int i = 0; i < length; ++i) {
            auto key = reader.ReadStringNoSeek();
            TitleImageList[key].Load(reader);
        }

        buffer = File::ReadAllBytes(path + "/TitleUI.dat");
        reader = StreamReader(buffer);
        TitleUI.resize(reader.ReadInt());
        for (size_t i = 0; i < TitleUI.size(); ++i) {
            TitleUI[i].Load(reader);
        }

        buffer = File::ReadAllBytes(path + "/TitleInOut.dat");
        reader = StreamReader(buffer);
        length = reader.ReadInt();
        for (int i = 0; i < length; ++i) {
            auto key = reader.ReadStringNoSeek();
            TitleInOut[key].Load(reader);
        }

    }
}
