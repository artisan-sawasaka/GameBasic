#include "DeviceManager.h"
#include "main/WinMain.h"

/*!
 * @brief 初期化
 */
void DeviceManager::Initialize(AppBase* app)
{
	app_ = app;
}

/*!
 * @brief ウインドウの幅取得
 */
int DeviceManager::GetWidth() const
{
	if (app_ == nullptr) return 0;
	return app_->GetWidth();
}

/*!
 * @brief ウインドウの高さ取得
 */
int DeviceManager::GetHeight() const
{
	if (app_ == nullptr) return 0;
	return app_->GetHeight();
}

/*!
 * @brief FPSの設定
 */
void DeviceManager::SetFPS(int fps)
{
	if (app_ == nullptr) return;
	app_->SetFPS(fps);
}

/*!
 * @brief 終了
 */
void DeviceManager::Exit()
{
	if (app_ == nullptr) return;
	PostMessage(app_->GetWindowHandle(), WM_CLOSE, 0, 0);
}

