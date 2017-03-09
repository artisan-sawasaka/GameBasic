/*!
 * @file
 *
 * @brief マウス管理クラスを提供します。
 *
 * @copyright
 */
#pragma once

#include <Windows.h>

/*!
 * @brief マウス管理クラス
 */
class Mouse
{
public :
	enum Button {
		LBUTTON,		//!< 左クリック
		MBUTTON,		//!< 中央クリック
		RBUTTON,		//!< 右クリック

		MAX,
	};

	/*!
	 * @brief マウスのクリア
	 */
	void Clear();

	/*!
	 * @brief イベント
	 */
	void WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	/*!
	 * @brief 更新
	 */
	void Update();

	/*!
	 * @brief ボタンの押されっぱなしを取得
	 */
	bool IsPress(Button button) const;

	/*!
	 * @brief ボタンのドラッグを取得
	 */
	bool IsDrag(Button button) const;

	/*!
	 * @brief ボタンの押した時を取得
	 */
	bool IsTrg(Button button) const;

	/*!
	 * @brief ボタンの離した時を取得
	 */
	bool IsRelease(Button button) const;

	/*!
	 * @brief ホイールが上に回されたときを取得
	 */
	bool IsHwheelUp() const;

	/*!
	 * @brief ホイールが下に回されたときを取得
	 */
	bool IsHwheelDown() const;

	/*!
	 * @brief マウスのX座標を取得
	 */
	int GetX() const;

	/*!
	 * @brief マウスのY座標を取得
	 */
	int GetY() const;

	/*!
	 * @brief マウスのXの移動量を取得
	 */
	int GetMoveX() const;

	/*!
	 * @brief マウスのYの移動量を取得
	 */
	int GetMoveY() const;

	static Mouse* GetInstance() {
		static Mouse v;
		return &v;
	}

private :
	Mouse();

	struct Info {
		bool button;
		bool button_back;
		bool button_temp;
	};
	Info infos_[MAX];

	int pos_x_;
	int pos_y_;
	int pos_x_back_;
	int pos_y_back_;
	int z_delta_;
	int wheel_fraction_;
	int notch_;
	HWND hwnd_;
};
