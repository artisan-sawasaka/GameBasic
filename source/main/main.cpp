#include "main.h"
#include "utility/SceneManager.h"
#include "utility/Renderer.h"
#include "utility/DeviceManager.h"
#include "utility/FadeManager.h"
#include "utility/ConsoleManager.h"
#include "utility/KeyManager.h"
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
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

/*!
 * @brief 継承した初期化
 */
void MainApp::Initialize()
{
	// レンダラーを初期化
	Renderer::GetInstance()->Initialize(GetBackHDC());

	// デバイスマネージャーの初期化
	DeviceManager::GetInstance()->Initialize(this);

	// シーン切り替え
	SceneManager::GetInstance()->Change(SceneList::Initialize, nullptr);
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
	// キーの更新
	KeyManager::GetInstance()->Update();

	// デバッグ機能
	if (KeyManager::GetInstance()->IsTrg('R')) {
		// 現在のシーンの再読み込み
		SoundManager::GetInstance()->StopAll();
		SceneManager::GetInstance()->Restart();
	} else if (KeyManager::GetInstance()->IsTrg('O')) {
		// デバッグ出力ウインドウを開く
		ConsoleManager::GetInstance()->Open();
	} else if (KeyManager::GetInstance()->IsTrg('C')) {
		// デバッグ出力ウインドウを閉じる
		ConsoleManager::GetInstance()->Close();
	} else if (KeyManager::GetInstance()->IsTrg('Q')) {
		is_debug_render_ = !is_debug_render_;
	}

	// 更新
	Update_(df);

	// 描画
	Render_();
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

void MainApp::Render_()
{
	// 画面クリア
	Renderer::GetInstance()->ClearScreen(Gdiplus::Color(0, 80, 255));

	// シーン
	SceneManager::GetInstance()->Render();

	// フェード
	FadeManager::GetInstance()->Render();

	// デバッグ表示
	RenderDebug_();

	// FPS表示
	Renderer::GetInstance()->DrawStringFormat(Renderer::RIGHT_TOP, GetWidth(), 0, 16, Gdiplus::Color::White, _T("FPS:%.1f"), GetAverageFPS());
}

void MainApp::RenderDebug_()
{
	if (!is_debug_render_) return;

	// 操作説明
	static const char* ds[] = {
		"R:シーンリセット",
		"O:コンソールを開く",
		"C:コンソールを閉じる",
		"Q:デバッグの表示/非表示",
	};
	for (int i = 0; i < sizeof(ds) / sizeof(*ds); ++i) {
		Renderer::GetInstance()->DrawString(ds[i], Renderer::LEFT_TOP, 0, i * 11, 9);
	}
}

