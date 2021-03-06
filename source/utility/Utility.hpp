#pragma once

#include <Windows.h>
#include <vector>
#include <string>
#include <set>
#include <cstdint>
#include <Shlwapi.h>
#include <memory>
#include "master/MasterData.hpp"
#include "Unpacker.hpp"
#include "render/Renderer.h"
#include "render/Texture.h"

class Utility
{
public:
	/*!
	 * @brief マスターデータの再読み込み
	 */
	static void ReloadMasterData()
	{
#if 1
		MasterData::Reload("data/master");
#else
		char current[MAX_PATH];
		GetCurrentDirectoryA(sizeof(current), current);
		SetCurrentDirectoryA("resource");
		system("call create_binary.bat");
		SetCurrentDirectoryA(current);
		MasterData::Reload("data/master");
#endif
	}

	/*!
	* @brief マスターデータの再読み込み
	*/
	static std::string StringFormat(const char* s, ...)
	{
		// テキストフォーマットを変換
		char text[10240];
		va_list arg;
		va_start(arg, s);
		vsprintf(text, s, arg);
		va_end(arg);

		return std::string(text);
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
		int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), -1, nullptr, 0);
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
	* @brief 色の設定
	*
	* @param data UIデータ
	* @param color 変更する色
	*/
	template <class Data>
	static void SetObjectColor(Data* data, const Color& color)
	{
		if (data == nullptr) return;
		data->r = color.GetR();
		data->g = color.GetG();
		data->b = color.GetB();
		data->a = color.GetA();
	}

	/*!
	 * @brief マスターデータからビットマップを読み込み
	 *
	 * @param ui UIデータ
	 *
	 * @return オブジェクトデータ
	 */
	template <class ImageList>
	static std::map<std::string, std::shared_ptr<Texture>> CreateBitmaps(const ImageList& list) {
		std::map<std::string, std::shared_ptr<Texture>> textures;
		std::set<std::string> images;
		Unpacker unpacker;
		for (auto it = list.begin(); it != list.end(); ++it) {
			images.insert(it->second.path);
		}
		unpacker.Set("data/image/image.dat");
		for (auto it = images.begin(); it != images.end(); ++it) {
			const auto& v = unpacker.GetData(*it);
			if (v.empty()) continue;
			Texture* texture = new Texture();
			if (texture == nullptr) continue;
			if (texture->CreateFromMemory(&v[0], v.size())) {
				textures[*it].reset(texture);
			} else {
				delete texture;
			}
		}
		return textures;
	}

	/*!
	 * @brief マスターデータから画像と文字列を描画
	 *
	 * @param ui UIデータ
	 *
	 * @return オブジェクトデータ
	 */
	template <class ImageList, class UI>
	static void BasicRender(const ImageList& list, const UI& ui, std::map<std::string, std::shared_ptr<Texture>>& textures)
	{
		auto render = Renderer::GetInstance();
		render->SetBlend(Renderer::BLEND_ALPHA);
		for (auto it = ui.begin(); it != ui.end(); ++it) {
			const auto& info = *it;
			if (!info.visible) continue;

			if (info.type == 0) {
				// 画像描画
				auto it2 = list.find(info.str);
				if (it2 == list.end()) continue ;
				const auto& info2 = it2->second;
				auto it3 = textures.find(info2.path);
				if (it3 == textures.end()) continue ;
				render->DrawImage(it3->second.get(),
					static_cast<Renderer::Anchor>(info.anchor),
					info.x,  info.y,  info.w,  info.h,
					info2.x, info2.y, info2.w, info2.h,
					Color(info.a, info.r, info.g, info.b),
					info.rotate);
			} else if (info.type == 1) {
				// 文字列描画
				render->DrawString(info.str.c_str(),
					static_cast<Renderer::Anchor>(info.anchor), info.x, info.y, info.h, 
					Color(info.a, info.r, info.g, info.b));
			}
		}
	}
	static uint32_t GetCrc32(const void* buf, uint32_t size, uint32_t crc32 = 0xffffffff)
	{
		static const uint32_t CRC32Table[] = {
			0x00000000, 0x04C11DB7, 0x09823B6E, 0x0D4326D9,
			0x130476DC, 0x17C56B6B, 0x1A864DB2, 0x1E475005,
			0x2608EDB8, 0x22C9F00F, 0x2F8AD6D6, 0x2B4BCB61,
			0x350C9B64, 0x31CD86D3, 0x3C8EA00A, 0x384FBDBD,
			0x4C11DB70, 0x48D0C6C7, 0x4593E01E, 0x4152FDA9,
			0x5F15ADAC, 0x5BD4B01B, 0x569796C2, 0x52568B75,
			0x6A1936C8, 0x6ED82B7F, 0x639B0DA6, 0x675A1011,
			0x791D4014, 0x7DDC5DA3, 0x709F7B7A, 0x745E66CD,
			0x9823B6E0, 0x9CE2AB57, 0x91A18D8E, 0x95609039,
			0x8B27C03C, 0x8FE6DD8B, 0x82A5FB52, 0x8664E6E5,
			0xBE2B5B58, 0xBAEA46EF, 0xB7A96036, 0xB3687D81,
			0xAD2F2D84, 0xA9EE3033, 0xA4AD16EA, 0xA06C0B5D,
			0xD4326D90, 0xD0F37027, 0xDDB056FE, 0xD9714B49,
			0xC7361B4C, 0xC3F706FB, 0xCEB42022, 0xCA753D95,
			0xF23A8028, 0xF6FB9D9F, 0xFBB8BB46, 0xFF79A6F1,
			0xE13EF6F4, 0xE5FFEB43, 0xE8BCCD9A, 0xEC7DD02D,
    
			0x34867077, 0x30476DC0, 0x3D044B19, 0x39C556AE,
			0x278206AB, 0x23431B1C, 0x2E003DC5, 0x2AC12072,
			0x128E9DCF, 0x164F8078, 0x1B0CA6A1, 0x1FCDBB16,
			0x018AEB13, 0x054BF6A4, 0x0808D07D, 0x0CC9CDCA,
			0x7897AB07, 0x7C56B6B0, 0x71159069, 0x75D48DDE,
			0x6B93DDDB, 0x6F52C06C, 0x6211E6B5, 0x66D0FB02,
			0x5E9F46BF, 0x5A5E5B08, 0x571D7DD1, 0x53DC6066,
			0x4D9B3063, 0x495A2DD4, 0x44190B0D, 0x40D816BA,
			0xACA5C697, 0xA864DB20, 0xA527FDF9, 0xA1E6E04E,
			0xBFA1B04B, 0xBB60ADFC, 0xB6238B25, 0xB2E29692,
			0x8AAD2B2F, 0x8E6C3698, 0x832F1041, 0x87EE0DF6,
			0x99A95DF3, 0x9D684044, 0x902B669D, 0x94EA7B2A,
			0xE0B41DE7, 0xE4750050, 0xE9362689, 0xEDF73B3E,
			0xF3B06B3B, 0xF771768C, 0xFA325055, 0xFEF34DE2,
			0xC6BCF05F, 0xC27DEDE8, 0xCF3ECB31, 0xCBFFD686,
			0xD5B88683, 0xD1799B34, 0xDC3ABDED, 0xD8FBA05A,
    
			0x690CE0EE, 0x6DCDFD59, 0x608EDB80, 0x644FC637,
			0x7A089632, 0x7EC98B85, 0x738AAD5C, 0x774BB0EB,
			0x4F040D56, 0x4BC510E1, 0x46863638, 0x42472B8F,
			0x5C007B8A, 0x58C1663D, 0x558240E4, 0x51435D53,
			0x251D3B9E, 0x21DC2629, 0x2C9F00F0, 0x285E1D47,
			0x36194D42, 0x32D850F5, 0x3F9B762C, 0x3B5A6B9B,
			0x0315D626, 0x07D4CB91, 0x0A97ED48, 0x0E56F0FF,
			0x1011A0FA, 0x14D0BD4D, 0x19939B94, 0x1D528623,
			0xF12F560E, 0xF5EE4BB9, 0xF8AD6D60, 0xFC6C70D7,
			0xE22B20D2, 0xE6EA3D65, 0xEBA91BBC, 0xEF68060B,
			0xD727BBB6, 0xD3E6A601, 0xDEA580D8, 0xDA649D6F,
			0xC423CD6A, 0xC0E2D0DD, 0xCDA1F604, 0xC960EBB3,
			0xBD3E8D7E, 0xB9FF90C9, 0xB4BCB610, 0xB07DABA7,
			0xAE3AFBA2, 0xAAFBE615, 0xA7B8C0CC, 0xA379DD7B,
			0x9B3660C6, 0x9FF77D71, 0x92B45BA8, 0x9675461F,
			0x8832161A, 0x8CF30BAD, 0x81B02D74, 0x857130C3,
    
			0x5D8A9099, 0x594B8D2E, 0x5408ABF7, 0x50C9B640,
			0x4E8EE645, 0x4A4FFBF2, 0x470CDD2B, 0x43CDC09C,
			0x7B827D21, 0x7F436096, 0x7200464F, 0x76C15BF8,
			0x68860BFD, 0x6C47164A, 0x61043093, 0x65C52D24,
			0x119B4BE9, 0x155A565E, 0x18197087, 0x1CD86D30,
			0x029F3D35, 0x065E2082, 0x0B1D065B, 0x0FDC1BEC,
			0x3793A651, 0x3352BBE6, 0x3E119D3F, 0x3AD08088,
			0x2497D08D, 0x2056CD3A, 0x2D15EBE3, 0x29D4F654,
			0xC5A92679, 0xC1683BCE, 0xCC2B1D17, 0xC8EA00A0,
			0xD6AD50A5, 0xD26C4D12, 0xDF2F6BCB, 0xDBEE767C,
			0xE3A1CBC1, 0xE760D676, 0xEA23F0AF, 0xEEE2ED18,
			0xF0A5BD1D, 0xF464A0AA, 0xF9278673, 0xFDE69BC4,
			0x89B8FD09, 0x8D79E0BE, 0x803AC667, 0x84FBDBD0,
			0x9ABC8BD5, 0x9E7D9662, 0x933EB0BB, 0x97FFAD0C,
			0xAFB010B1, 0xAB710D06, 0xA6322BDF, 0xA2F33668,
			0xBCB4666D, 0xB8757BDA, 0xB5365D03, 0xB1F740B4,
		};
		const uint8_t* p = reinterpret_cast<const uint8_t*>(buf);
		while (size != 0) {
			crc32 = CRC32Table[(crc32 >> (32 - CHAR_BIT)) ^ *p] ^ (crc32 << CHAR_BIT);
			++p;
			--size;
		}

		return crc32;
	}

	/*!
	 * @brief フルパスからファイル名なしのパスを取得します
	 */
	static std::string GetDirectoryName(const char* str)
	{
		const char* p = strrchr(str, '\\');
		if (p == NULL) {
			p = strrchr(str, '/');
			if (p == NULL) {
				return std::string(str);
			}
		}
		return std::string(str, p - str);
	}

	/*!
	 * @brief 指定したパス文字列のファイル名と拡張子を返します。
	 */
	static std::string GetFileName(const char* str)
	{
		const char* p = strrchr(str, '\\');
		if (p == NULL) {
			p = strrchr(str, '/');
			if (p == NULL) {
				return std::string(str);
			}
		}
		return std::string(p + 1);
	}

	/*!
	 * @brief 指定したパス文字列のファイル名を拡張子を付けずに返します。
	 */
	static std::string GetFileNameWithoutExtension(const char* str)
	{
		const char* p = strrchr(str, '\\');
		if (p == NULL) {
			p = strrchr(str, '/');
		}
		if (p == NULL){
			p = str;
		} else {
			p++;
		}
		const char* pp = strrchr(p, '.');
		if (pp == NULL) {
			return std::string(p);
		}
		return std::string(p, pp - p);
	}

	/*!
	 * @brief 指定したパス文字列の拡張子を返します。
	 */
	static std::string GetExtension(const char* str)
	{
		const char* pp = strrchr(str, '.');
		if (pp == NULL) {
			return std::string(str);
		}
		return std::string(pp + 1);
	}
};
