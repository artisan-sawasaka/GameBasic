#include <Windows.h>
#include "Key.h"
#include <string.h>
#include <algorithm>
#include "master/MasterData.hpp"

static unsigned char changeTable[256] = 
{
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',
	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',
	VK_F1,
	VK_F2,
	VK_F3,
	VK_F4,
	VK_F5,
	VK_F6,
	VK_F7,
	VK_F8,
	VK_F9,
	VK_F10,
	VK_F11,
	VK_F12,
	VK_BACK,
	VK_TAB,
	VK_CLEAR,
	VK_RETURN,
	VK_SHIFT,
	VK_CONTROL,
	VK_MENU,
	VK_PAUSE,
	VK_CAPITAL,
	VK_KANA,
	VK_HANGEUL,
	VK_HANGUL,
	VK_JUNJA,
	VK_FINAL,
	VK_HANJA,
	VK_KANJI,
	VK_ESCAPE,
	VK_CONVERT,
	VK_NONCONVERT,
	VK_ACCEPT,
	VK_MODECHANGE,
	VK_SPACE,
	VK_PRIOR,
	VK_NEXT,
	VK_END,
	VK_HOME,
	VK_LEFT,
	VK_UP,
	VK_RIGHT,
	VK_DOWN,
	VK_SELECT,
	VK_PRINT,
	VK_EXECUTE,
	VK_SNAPSHOT,
	VK_INSERT,
	VK_DELETE,
	VK_HELP,
};

Key::Key()
{
	Clear();
}

/*!
 * @brief クリア
 */
void Key::Clear()
{
	memset(infos_, false , sizeof(infos_));
}

/*!
 * @brief 押した時
 */
void Key::Down(unsigned char key)
{
	infos_[key].key_temp = true;
}

/*!
 * @brief 離した時
 */
void Key::Up(unsigned char key)
{
	infos_[key].key_temp = false;
}

/*!
 * @brief キーの更新
 */
void Key::Update()
{
	for (int i = 0; i < sizeof(infos_) / sizeof(*infos_); ++i) {
		auto& info = infos_[i];
		info.key_back = info.key;
		info.key = info.key_temp;
		info.key_count = info.key ? info.key_count + 1 : 0;
	}
}

/*!
 * @brief キーの押されっぱなしを取得
 */
bool Key::IsPress(KeyCode::Code code) const
{
	return infos_[changeTable[code]].key;
}

/*!
 * @brief キーの押した時を取得
 */
bool Key::IsTrg(KeyCode::Code code) const
{
	return infos_[changeTable[code]].key && !infos_[changeTable[code]].key_back;
}

/*!
 * @brief キーの離した時を取得
 */
bool Key::IsRelease(KeyCode::Code code) const
{
	return !infos_[changeTable[code]].key && infos_[changeTable[code]].key_back;
}

/*!
 * @brief キーリピート
 */
bool Key::IsRepeat(KeyCode::Code code) const
{
	auto& info = infos_[changeTable[code]];
	if (info.key_count == 0) return false;
	if (info.key_count >= MasterData::KeyRepeatBase[MasterData::KeyRepeatBase.size() - 1].value) {
		return info.key_count % 2 == 0;
	}
	for (size_t i = 0; i < MasterData::KeyRepeatBase.size(); ++i) {
		if (info.key_count == MasterData::KeyRepeatBase[i].value) {
			return true;
		}
	}
	return false;
}
