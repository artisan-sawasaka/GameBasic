#include "main.h"
#include "utility/SceneManager.h"
#include "utility/Renderer.h"
#include "utility/DeviceManager.h"
#include "utility/FadeManager.h"
#include "utility/ConsoleManager.h"
#include "utility/KeyManager.h"
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
	if (KeyManager::GetInstance()->IsTrg('P')) {
		// 現在のシーンの再読み込み
		SceneManager::GetInstance()->Restart();
	} else if (KeyManager::GetInstance()->IsTrg('O')) {
		// デバッグ出力ウインドウを開く
		ConsoleManager::GetInstance()->Open();
	} else if (KeyManager::GetInstance()->IsTrg('C')) {
		// デバッグ出力ウインドウを閉じる
		ConsoleManager::GetInstance()->Close();
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
}

void MainApp::Render_()
{
	// 画面クリア
	Renderer::GetInstance()->ClearScreen(Gdiplus::Color(0, 80, 255));

	// シーン
	SceneManager::GetInstance()->Render();

	// フェード
	FadeManager::GetInstance()->Render();

	// FPS表示
	Renderer::GetInstance()->DrawStringFormat(Renderer::RIGHT_TOP, GetWidth(), 0, 16, Gdiplus::Color::White, _T("FPS:%.1f"), GetAverageFPS());
}
