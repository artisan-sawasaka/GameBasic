/*!
 * @file
 *
 * @brief キー管理クラスを提供します。
 *
 * @copyright
 */
#pragma once

/*!
 * @brief キー管理クラス
 */
class KeyManager
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
	bool IsPress(unsigned char key) const;

	/*!
	 * @brief キーの押した時を取得
	 */
	bool IsTrg(unsigned char key) const;

	/*!
	 * @brief キーの離した時を取得
	 */
	bool IsRelease(unsigned char key) const;

	/*!
	 * @brief キーリピート
	 */
	bool IsRepeat(unsigned char key) const;

	static KeyManager* GetInstance() {
		static KeyManager v;
		return &v;
	}

private :
	KeyManager();

	struct Info {
		bool key;
		bool key_back;
		bool key_temp;
		int key_count;
	};
	Info keys_[256];
};
