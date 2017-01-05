#pragma once

#include "WinMain.h"

class MainApp : public AppBase
{
public :
	MainApp(TCHAR* wnClassName, TCHAR* title);
	~MainApp();

	/*!
	 * @brief 継承したウインドプロシージャ
	 */
	virtual LRESULT WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam);

	/*!
	 * @brief 継承した初期化
	 */
	virtual void Initialize();

	/*!
	 * @brief 継承した終了化
	 */
	virtual void Finalize();

	/*!
	 * @brief 継承したメインループ
	 */
	virtual void Update(float dt);

private :
	void Update_(float df);
	void Render_();
	void RenderDebug_();

	bool is_debug_render_;
};
