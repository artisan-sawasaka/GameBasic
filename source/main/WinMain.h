#pragma once

#include <Windows.h>
#include <tchar.h>
#include <vector>
#include <gdiplus.h>
#include <memory>

class AppBase
{
public :
	AppBase(TCHAR* wnClassName, TCHAR* title);
	virtual ~AppBase();

	/*!
	 * @brief ウインドウの座標を変更
	 */
	void SetWindowPosition(int x, int y);

	/*!
	 * @brief ウインドウのサイズを変更
	 */
	void SetWindowSize(int w, int h);

	/*!
	 * @brief ウインドウのX座標取得
	 */
	int GetPostionX() const { return wnd_x_; }

	/*!
	 * @brief ウインドウのY座標取得
	 */
	int GetPostionY() const { return wnd_y_; }

	/*!
	 * @brief ウインドウの幅取得
	 */
	int GetWidth() const { return wnd_w_; }

	/*!
	 * @brief ウインドウの高さ取得
	 */
	int GetHeight() const { return wnd_h_; }

	/*!
	 * @brief ウインドウのハンドル取得
	 */
	HWND GetWindowHandle() const { return hwnd_; }

	/*!
	 * @brief FPSの設定
	 */
	void SetFPS(int fps) { fps_ = fps; }

	/*!
	 * @brief FPSの取得
	 */
	int GetFPS() const { return fps_; }

	/*!
	 * @brief 平均FPSの取得
	 */
	float GetAverageFPS() const { return average_fps_; }

	/*!
	 * @brief バックバッファのHDC取得
	 */
	HDC GetBackHDC() const { return hdc_mem_; }
	
	/*!
	 * @brief 開始
	 */
	bool Start();

public :
	/*!
	 * @brief ウインドプロシージャの仮想関数
	 */
	virtual LRESULT WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam) = 0;

	/*!
	 * @brief 初期化
	 */
	virtual void Initialize() = 0;

	/*!
	 * @brief 終了化
	 */
	virtual void Finalize() = 0;

	/*!
	 * @brief メインループ
	 */
	virtual void Update(float df) = 0;

private :
	static LRESULT CALLBACK WndProc_(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	/*!
	 * @brief クライアント領域のサイズを取得
	 */
	SIZE GetClientSize_(int w, int h);

	/*!
	 * @brief 垂直動機待ち
	 */
	float VSync_();

	/*!
	 * @brief 描画処理
	 */
	void Render_();

	TCHAR wndClassName_[256];
	TCHAR title_[256];
	TIMECAPS timer_caps_;
	WNDCLASSEX wcex_;
	int wnd_x_;
	int wnd_y_;
	int wnd_w_;
	int wnd_h_;
	float average_fps_;
	int fps_;
	DWORD fps_count_;
	DWORD old_time_;
	DWORD start_time_;
	HWND hwnd_;
	DWORD style_;

	// バックバッファ
	HBITMAP bitmap_;
	HDC hdc_mem_;
};
