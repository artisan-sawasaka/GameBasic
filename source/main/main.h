#pragma once

#include <memory>
#include <map>
#include "WinMain.h"
#include "utility/counter.h"
#include "utility/bezier.h"
#include "master/MasterData.hpp"

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
	/*!
	 * @brief マスター再読み込み
	 */
	void Reload_();

	void SetBezier_(const Bezier::ControlPoint& cp);


	std::map<std::string, std::shared_ptr<Gdiplus::Bitmap>> bitmaps_;
	std::map<std::string, MasterData::TitleUIData*> objects_;

	SaturationCounter stop_counter_;
	RootCounter root_counter_;
	InteriorCounter fade_counter_;
	LoopCounter cursor_counter_;
	Bezier::Counter<int> bezier_counter_;
	Bezier::Timer<int> bezier_timer_;

	int cursour_y_;
};
