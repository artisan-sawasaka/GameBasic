#include "Mouse.h"
#include <string.h>
#include <algorithm>
#include "master/MasterData.hpp"

Mouse::Mouse()
	: hwnd_(NULL)
{
	Clear();
}

/*!
 * @brief クリア
 */
void Mouse::Clear()
{
	memset(infos_, false , sizeof(infos_));
	pos_x_ = 0;
	pos_y_ = 0;
	pos_x_back_ = 0;
	pos_y_back_ = 0;
	z_delta_ = 0;
	wheel_fraction_ = 0;
	notch_ = 0;
}

/*!
 * @brief イベント
 */
void Mouse::WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	hwnd_ = hWnd;
	if (msg == WM_LBUTTONDOWN) {
		infos_[LBUTTON].button_temp = true;
		SetCapture(hWnd);
	} else if (msg == WM_LBUTTONUP) {
		infos_[LBUTTON].button_temp = false;
		ReleaseCapture();
	} else if (msg == WM_MBUTTONDOWN) {
		infos_[MBUTTON].button_temp = true;
		SetCapture(hWnd);
	} else if (msg == WM_MBUTTONUP) {
		infos_[MBUTTON].button_temp = false;
		ReleaseCapture();
	} else if (msg == WM_RBUTTONDOWN) {
		infos_[RBUTTON].button_temp = true;
		SetCapture(hWnd);
	} else if (msg == WM_RBUTTONUP) {
		infos_[RBUTTON].button_temp = false;
		ReleaseCapture();
	} else if (msg == WM_MOUSEWHEEL) {
		z_delta_ = GET_WHEEL_DELTA_WPARAM(wParam) + wheel_fraction_;
		wheel_fraction_ = z_delta_ % WHEEL_DELTA;
	}
}

/*!
 * @brief ボタンの更新
 */
void Mouse::Update()
{
	for (int i = 0; i < sizeof(infos_) / sizeof(*infos_); ++i) {
		auto& info = infos_[i];
		info.button_back = info.button;
		info.button = info.button_temp;
	}
	notch_ = z_delta_ / WHEEL_DELTA;
	z_delta_ = 0;

	POINT pos;
	if (GetCursorPos(&pos)) {
		if (ScreenToClient(hwnd_, &pos)) {
			pos_x_back_ = pos_x_;
			pos_y_back_ = pos_y_;
			pos_x_ = pos.x;
			pos_y_ = pos.y;
		}
	}
}

/*!
 * @brief ボタンの押されっぱなしを取得
 */
bool Mouse::IsPress(Button button) const
{
	return infos_[button].button;
}

/*!
 * @brief ボタンのドラッグを取得
 */
bool Mouse::IsDrag(Button button) const
{
	return IsPress(button) && !IsTrg(button);
}

/*!
 * @brief ボタンの押した時を取得
 */
bool Mouse::IsTrg(Button button) const
{
	return infos_[button].button && !infos_[button].button_back;
}

/*!
 * @brief ボタンの離した時を取得
 */
bool Mouse::IsRelease(Button button) const
{
	return !infos_[button].button && infos_[button].button_back;
}

/*!
 * @brief ホイールが上に回されたときを取得
 */
bool Mouse::IsHwheelUp() const
{
	return notch_ > 0;
}

/*!
 * @brief ホイールが下に回されたときを取得
 */
bool Mouse::IsHwheelDown() const
{
	return notch_ < 0;
}

/*!
 * @brief マウスのX座標を取得
 */
int Mouse::GetX() const
{
	return pos_x_;
}

/*!
 * @brief マウスのY座標を取得
 */
int Mouse::GetY() const
{
	return pos_y_;
}

/*!
 * @brief マウスのXの移動量を取得
 */
int Mouse::GetMoveX() const
{
	return pos_x_ - pos_x_back_;
}

/*!
 * @brief マウスのYの移動量を取得
 */
int Mouse::GetMoveY() const
{
	return pos_y_ - pos_y_back_;
}
