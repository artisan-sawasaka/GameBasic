#include "MasterData.hpp"
#include "utility/FileUtility.hpp"
#include "utility/StreamReader.hpp"

namespace MasterData
{
    std::map<std::string, TitleImageListData> TitleImageList;
    std::vector<TitleUIData> TitleUI;
    ConstData Const;


    void Reload(const std::string& path)
    {
        std::vector<char> buffer;
	    StreamReader reader;
        int length;

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

        buffer = File::ReadAllBytes(path + "/Const.dat");
        reader = StreamReader(buffer);
        Const.Load(reader);

    }
}
