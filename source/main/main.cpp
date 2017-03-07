#include "main.h"
#include "render/Renderer.h"
#include "utility/SceneManager.h"
#include "utility/DeviceManager.h"
#include "utility/FadeManager.h"
#include "utility/ConsoleManager.h"
#include "utility/KeyManager.h"
#include "utility/MouseManager.h"
#include "utility/Utility.hpp"
#include "sound/SoundManager.h"
#include <set>
#include <algorithm>

/*!
 * @brief WinMain
 */
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	// クラス名とタイトルを設定
	MainApp app(_T("GameBasic"), _T("ゲーム開発のベース"));

	// アプリをスタート
	app.Start();

	return 0;
}
MainApp::MainApp(TCHAR* wnClassName, TCHAR* title)
	: AppBase(wnClassName, title)
	, is_debug_render_(true)
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
	if (msg == WM_KEYDOWN) {
		KeyManager::GetInstance()->Down(wParam);
	} else if (msg == WM_KEYUP) {
		KeyManager::GetInstance()->Up(wParam);
	} else if (msg == WM_ACTIVATE && wParam == 0) {
		KeyManager::GetInstance()->Clear();
		MouseManager::GetInstance()->Clear();
	} else if (msg == WM_SYSKEYDOWN) {
		if (wParam == VK_RETURN) {
		}
	}
	MouseManager::GetInstance()->WndProc(hWnd, msg, wParam, lParam);

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*!
 * @brief 継承した初期化
 */
void MainApp::Initialize()
{
	// レンダラーを初期化
	Renderer::GetInstance()->Initialize(this);

	// デバイスマネージャーの初期化
	DeviceManager::GetInstance()->Initialize(this);

	// マスターデータ読み込み
	ReloadMasterData_();

	// シーン切り替え
	SceneManager::GetInstance()->Change(SceneList::Initialize, nullptr);

}

/*!
 * @brief 継承した終了化
 */
void MainApp::Finalize()
{
	// シーンの終了
	SceneManager::GetInstance()->Finalize();

	// レンダラーを終了化
	Renderer::GetInstance()->Finalize();
}

/*!
 * @brief 継承したメインループ
 */
void MainApp::Update(float df)
{
	// キーの更新
	KeyManager::GetInstance()->Update();

	// マウス更新
	MouseManager::GetInstance()->Update();

	// カメラの更新
	camera_.Update(df);

	// デバッグ機能
	if (KeyManager::GetInstance()->IsTrg('R')) {
		// 現在のシーンの再読み込み
		SoundManager::GetInstance()->StopAll();
		ReloadMasterData_();
		SceneManager::GetInstance()->Restart();
	} else if (KeyManager::GetInstance()->IsTrg('O')) {
		// デバッグ出力ウインドウを開く
		//ConsoleManager::GetInstance()->Open();
	} else if (KeyManager::GetInstance()->IsTrg('C')) {
		// デバッグ出力ウインドウを閉じる
		//ConsoleManager::GetInstance()->Close();
	} else if (KeyManager::GetInstance()->IsTrg('Q')) {
		//is_debug_render_ = !is_debug_render_;
	}

	// 更新
	Update_(df);
}

void MainApp::Update_(float df)
{
	// フェードの更新
	FadeManager::GetInstance()->Update(df);

	// シーンの更新
	SceneManager::GetInstance()->Update(df);

	// サウンドの更新
	SoundManager::GetInstance()->Update(df);
}

/*!
 * @brief 画面クリア
 */
void MainApp::ClearScreen()
{
	// 画面クリア
	//Renderer::GetInstance()->ClearScreen(Color(255, 0, 255));
	Renderer::GetInstance()->ClearScreen(Color(0, 0, 0));
}

/*!
 * @brief 3Dレンダリング
 */
void MainApp::Render3D()
{
	// シーン
	SceneManager::GetInstance()->Render3D();
}

/*!
 * @brief 2Dレンダリング
 */
void MainApp::Render2D()
{
	// シーン
	SceneManager::GetInstance()->Render2D();

	// フェード
	FadeManager::GetInstance()->Render();

	// デバッグ表示
	RenderDebug_();
}

void MainApp::RenderDebug_()
{
	if (!is_debug_render_) return;

	/*
	// 操作説明
	const std::string ds[] = {
		camera_.GetInfo(),
		"R:シーンリセット",
		"O:コンソールを開く",
		"C:コンソールを閉じる",
		"Q:デバッグの表示/非表示",
	};
	for (int i = 0; i < sizeof(ds) / sizeof(*ds); ++i) {
		Renderer::GetInstance()->DrawString(ds[i].c_str(), Renderer::LEFT_TOP, 0, i * 12, 12);
	}
	*/

	// FPS表示
	Renderer::GetInstance()->DrawStringFormat(Renderer::RIGHT_TOP, GetWidth(), 0, 12, Color::White, _T("FPS:%.1f"), GetAverageFPS());
}

void MainApp::ReloadMasterData_()
{
	Utility::ReloadMasterData();
	DeviceManager::GetInstance()->SetFPS(MasterData::Const.FPS);
	SetWindowSize(MasterData::Const.window_width, MasterData::Const.window_height);
	GetDevice().SetBackBufferSize(MasterData::Const.backbuffer_width, MasterData::Const.backbuffer_height);
}
