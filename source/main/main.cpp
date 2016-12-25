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
	if (msg == WM_KEYDOWN) {
		KeyManager::GetInstance()->Down(wParam);
	} else if (msg == WM_KEYUP) {
		KeyManager::GetInstance()->Up(wParam);
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

	ConsoleManager::GetInstance()->Open();
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

	// シーンの再読み込み
	if (KeyManager::GetInstance()->IsTrg('P')) {
		SceneManager::GetInstance()->Restart();
	}

	// フェードの更新
	FadeManager::GetInstance()->Update(df);

	// シーンの更新
	SceneManager::GetInstance()->Update(df);

	// フェード処理
	FadeManager::GetInstance()->Render();

	// FPS表示
	Renderer::GetInstance()->DrawStringFormat(Renderer::RIGHT_TOP, GetWidth(), 0, 16, Gdiplus::Color::White, _T("FPS:%.1f"), GetAverageFPS());
}
