#pragma once

#include <Windows.h>
#include <vector>
#include <string>

class Utility
{
public:
	/*!
	 * @brief マスターデータの再読み込み
	 */
	static void ReloadMasterData()
	{
		char current[MAX_PATH];
		GetCurrentDirectoryA(sizeof(current), current);
		SetCurrentDirectoryA("resource");
		system("call create_binary.bat");
		SetCurrentDirectoryA(current);
		MasterData::Reload("data/master");
	}

	/*!
	 * @brief SJISをUTF16に変換
	 *
	 * @param path ファイルのパス
	 *
	 * @return 読み込んだバイナリ
	 */
	static std::wstring SJIStoUTF16(const std::string& s)
	{
		int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, NULL, 0);
		std::vector<wchar_t> v(len);
		MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, &v[0], v.size());
		return std::wstring(v.begin(), v.begin() + len - 1);
	}

	/*!
	 * @brief マスターデータを操作しやすい形にオブジェクトに変換
	 *
	 * @param ui UIデータ
	 *
	 * @return オブジェクトデータ
	 */
	template <class Data, class UI>
	static std::map<std::string, Data*> CreateObjects(UI& ui) {
		std::map<std::string, Data*> ret;
		for (auto it = ui.begin(); it != ui.end(); ++it) {
			ret.insert(std::pair<std::string, Data*>(it->name, &(*it)));
		}
		return ret;
	}

	/*!
	 * @brief マスターデータからビットマップを読み込み
	 *
	 * @param ui UIデータ
	 *
	 * @return オブジェクトデータ
	 */
	template <class ImageList>
	static std::map<std::string, std::shared_ptr<Gdiplus::Bitmap>> CreateBitmaps(const ImageList& list) {
		std::map<std::string, std::shared_ptr<Gdiplus::Bitmap>> bitmaps;
		std::set<std::string> images;
		for (auto it = list.begin(); it != list.end(); ++it) {
			images.insert(it->second.path);
		}
		for (auto it = images.begin(); it != images.end(); ++it) {
			std::string path = std::string("data/image/") + *it;
			auto bmp = new Gdiplus::Bitmap(Utility::SJIStoUTF16(path).c_str());
			if (bmp->GetLastStatus() == Gdiplus::Ok) {
				bitmaps[*it].reset(bmp);
			} else {
				delete bmp;
			}
		}
		return bitmaps;
	}

	/*!
	 * @brief マスターデータから画像と文字列を描画
	 *
	 * @param ui UIデータ
	 *
	 * @return オブジェクトデータ
	 */
	template <class ImageList, class UI>
	static void BaseRender(const ImageList& list, const UI& ui, std::map<std::string, std::shared_ptr<Gdiplus::Bitmap>>& bitmaps)
	{
		for (auto it = ui.begin(); it != ui.end(); ++it) {
			const auto& info = *it;
			if (info.type == 0) {
				// 画像描画
				auto it2 = list.find(info.str);
				if (it2 == list.end()) continue ;
				const auto& info2 = it2->second;
				auto it3 = bitmaps.find(info2.path);
				if (it3 == bitmaps.end()) continue ;

				Renderer::GetInstance()->DrawImage(it3->second.get(),
					static_cast<Renderer::Anchor>(info.anchor),
					info.x,  info.y,  info.w,  info.h,
					info2.x, info2.y, info2.w, info2.h,
					Gdiplus::Color(info.a, info.r, info.g, info.b));
			} else if (info.type == 1) {
				// 文字列描画
				Renderer::GetInstance()->DrawString(info.str.c_str(),
					static_cast<Renderer::Anchor>(info.anchor), info.x, info.y, info.h, 
					Gdiplus::Color(info.a, info.r, info.g, info.b));
			}
		}
	}
};
