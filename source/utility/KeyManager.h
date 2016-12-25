#pragma once

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

	static KeyManager* GetInstance() {
		static KeyManager v;
		return &v;
	}

private :
	KeyManager();
	bool keys_[256];
	bool keys_back_[256];
	bool keys_temp_[256];
};
