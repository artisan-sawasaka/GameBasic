/*!
* @file
*
* @brief キー管理クラスを提供します。
*
* @copyright
*/
#pragma once

class KeyCode
{
public :
	enum Code {
		A,
		B,
		C,
		D,
		E,
		F,
		G,
		H,
		I,
		J,
		K,
		L,
		M,
		N,
		O,
		P,
		Q,
		R,
		S,
		T,
		U,
		V,
		W,
		X,
		Y,
		Z,
		N0,
		N1,
		N2,
		N3,
		N4,
		N5,
		N6,
		N7,
		N8,
		N9,
		F1,
		F2,
		F3,
		F4,
		F5,
		F6,
		F7,
		F8,
		F9,
		F10,
		F11,
		F12,
		BACK,
		TAB,
		CLEAR,
		ENTER,
		SHIFT,
		CONTROL,
		MENU,
		PAUSE,
		CAPITAL,
		KANA,
		HANGEUL,
		HANGUL,
		JUNJA,
		FINAL,
		HANJA,
		KANJI,
		ESCAPE,
		CONVERT,
		NONCONVERT,
		ACCEPT,
		MODECHANGE,
		SPACE,
		PRIOR,
		NEXT,
		END,
		HOME,
		LEFT,
		UP,
		RIGHT,
		DOWN,
		SELECT,
		PRINT,
		EXECUTE,
		SNAPSHOT,
		INSERT,
		DEL,
		HELP,
	};
};

/*!
* @brief キー管理クラス
*/
class Key
{
public :
	/*!
	* @brief キーのクリア
	*/
	void Clear();

	/*!
	* @brief 押した時
	*/
	void Down(unsigned char key);

	/*!
	* @brief 離した時
	*/
	void Up(unsigned char key);

	/*!
	* @brief キーの更新
	*/
	void Update();

	/*!
	* @brief キーの押されっぱなしを取得
	*/
	bool IsPress(KeyCode::Code code) const;

	/*!
	* @brief キーの押した時を取得
	*/
	bool IsTrg(KeyCode::Code code) const;

	/*!
	* @brief キーの離した時を取得
	*/
	bool IsRelease(KeyCode::Code code) const;

	/*!
	* @brief キーリピート
	*/
	bool IsRepeat(KeyCode::Code code) const;

	static Key* GetInstance() {
		static Key v;
		return &v;
	}

private :
	Key();

	struct Info {
		bool key;
		bool key_back;
		bool key_temp;
		int key_count;
	};
	Info infos_[256];
};
