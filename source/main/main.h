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
	
	/*!
	 * @brief 画面クリア
	 */
	virtual void ClearScreen();

	/*!
	 * @brief 3Dレンダリング
	 */
	virtual void Render3D();

	/*!
	 * @brief 2Dレンダリング
	 */
	virtual void Render2D();

private :
	void Update_(float df);
	void RenderDebug_();
	void ReloadMasterData_();

	bool is_debug_render_;
};
