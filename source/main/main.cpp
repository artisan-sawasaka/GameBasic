#include "main.h"
#include "utility/SceneManager.h"
#include "utility/Renderer.h"
#include "utility/DeviceManager.h"
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
	// シーンの再読み込み
	if (GetKeyState('P') < 0) {
		SceneManager::GetInstance()->Restart();
	}

	// シーンの更新
	SceneManager::GetInstance()->Update(df);

	// FPS表示
	Renderer::GetInstance()->DrawStringFormat(Renderer::RIGHT_TOP, GetWidth(), 0, 16, Gdiplus::Color::White, _T("FPS:%.1f"), GetAverageFPS());
}
