#include "main.h"
#include "utility/Renderer.h"
#include "utility/Utility.hpp"
#include <set>
#include <algorithm>

/*!
 * @brief WinMain
 */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// クラス名とタイトルを設定
	MainApp app(_T("bezier"), _T("bezierのテスト"));

	// アプリをスタート
	app.Start();

	return 0;
}

MainApp::MainApp(TCHAR* wnClassName, TCHAR* title)
	: AppBase(wnClassName, title)
{
}

MainApp::~MainApp()
{
}

/*!
 * @brief 継承したウインドプロシージャ
 */
LRESULT MainApp::WndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*!
 * @brief 継承した初期化
 */
void MainApp::Initialize()
{
	// レンダラーを初期化
	Renderer::GetInstance()->Initialize(GetBackHDC());

	// マスターデータ読み込み
	Reload_();
	
	// 各種カウンター初期化
	stop_counter_.set(0, GetWidth() - 150, 6);
	root_counter_.set(0, GetWidth(), 4);
	fade_counter_.set(0, 0, 1);
	cursor_counter_.set(0, 10, -3);
	SetBezier_(Bezier::Linear);
}

/*!
 * @brief 継承した終了化
 */
void MainApp::Finalize()
{
}

/*!
 * @brief 継承したメインループ
 */
void MainApp::Update(float df)
{
	auto c = GetKeyState('R');
	if (GetKeyState('R') < 0) {
		// マスターデータ再読み込み
		Reload_();
	} else if (GetKeyState('I') < 0) {
		// フェードイン
		fade_counter_.set(fade_counter_, 0, 30);
	} else if (GetKeyState('O') < 0) {
		// フェードアウト
		fade_counter_.set(fade_counter_, 255, 30);
	} else if (GetKeyState('S') < 0) {
		// 止まったカウンターを再度再生
		stop_counter_.set(0, stop_counter_.getEnd(), 6);
	} else if (GetKeyState('1') < 0) {
		SetBezier_(Bezier::Linear);
	} else if (GetKeyState('2') < 0) {
		SetBezier_(Bezier::EaseIn);
	} else if (GetKeyState('3') < 0) {
		SetBezier_(Bezier::EaseOut);
	} else if (GetKeyState('4') < 0) {
		SetBezier_(Bezier::EaseInOut);
	} else if (GetKeyState('5') < 0) {
		SetBezier_(Bezier::EaseInBack);
	} else if (GetKeyState('6') < 0) {
		SetBezier_(Bezier::EaseOutBack);
	} else if (GetKeyState('7') < 0) {
		SetBezier_(Bezier::EaseInOutBack);
	} else if (GetKeyState('8') < 0) {
		SetBezier_(Bezier::EaseInCirc);
	} else if (GetKeyState('9') < 0) {
		SetBezier_(Bezier::ControlPoint(MasterData::Const.x1, MasterData::Const.y1, MasterData::Const.x2, MasterData::Const.y2));
	}

	++stop_counter_;
	++root_counter_;
	++fade_counter_;
	++cursor_counter_;
	++bezier_counter_;
	bezier_timer_.Update(df);

	// とまる～を操作
	objects_["TextStop"]->x = stop_counter_;

	// まわる～を操作
	objects_["TextLoop"]->x = root_counter_;

	// カーソルを操作
	objects_["TextCursor"]->y = cursour_y_ + cursor_counter_;

	// ベジェカウンターを操作
	objects_["BezierCounter"]->x = bezier_counter_;

	// カーソルを操作
	objects_["BezierTimer"]->x = bezier_timer_;

	// 画面クリア
	Renderer::GetInstance()->ClearScreen(Gdiplus::Color(0, 80, 255));

	// マスターデータに基づく描画処理
	Utility::BaseRender(MasterData::TitleImageList, MasterData::TitleUI, bitmaps_);

	// フェード
	Renderer::GetInstance()->FillRect(0, 0, GetWidth(), GetHeight(), Gdiplus::Color(fade_counter_, 0, 0, 0));

	// 操作説明
	static const char* ds[] = {
		"R:マスター読み込み",
		"I:フェードイン",
		"O:フェードアウト",
		"S:とまる～を再度移動",
		"1:Bezier::Linear:",
		"2:Bezier::EaseIn:",
		"3:Bezier::EaseOut",
		"4:Bezier::EaseInOut",
		"5:Bezier::EaseInBack",
		"6:Bezier::EaseOutBack",
		"7:Bezier::EaseInOutBack",
		"8:Bezier::EaseInCirc",
		"9:Bezier オリジナル",
	};
	for (int i = 0; i < sizeof(ds) / sizeof(*ds); ++i) {
		Renderer::GetInstance()->DrawString(ds[i], Renderer::LEFT_TOP, 0, i * 16, 12);
	}

	// FPS表示
	Renderer::GetInstance()->DrawStringFormat(Renderer::RIGHT_TOP, GetWidth(), 0, 16, Gdiplus::Color::White, _T("FPS:%.1f"), GetAverageFPS());
}

/*!
 * @brief マスター再読み込み
 */
void MainApp::Reload_()
{
	// マスターデータを再読み込み
	Utility::ReloadMasterData();

	// FPSの設定
	SetFPS(MasterData::Const.FPS);

	// 画像を読み込み
	bitmaps_ = Utility::CreateBitmaps(MasterData::TitleImageList);

	// 操作しやすいようにデータ化する
	objects_ = Utility::CreateObjects<MasterData::TitleUIData>(MasterData::TitleUI);

	cursour_y_ = objects_["TextCursor"]->y;
}

void MainApp::SetBezier_(const Bezier::ControlPoint& cp)
{
	bezier_counter_.Set(100, 400, 60, cp);
	bezier_timer_.Set(100, 400, 1.0f, cp);
}
