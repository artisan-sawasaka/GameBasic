#pragma once

class AppBase;

class DeviceManager
{
public :
	/*!
	 * @brief 初期化
	 */
	void Initialize(AppBase* app); 

	/*!
	 * @brief ウインドウの幅取得
	 */
	int GetWidth() const;

	/*!
	 * @brief ウインドウの高さ取得
	 */
	int GetHeight() const;

	/*!
	 * @brief FPSの設定
	 */
	void SetFPS(int fps);

	static DeviceManager* GetInstance()
	{
		static DeviceManager v;
		return &v;
	}
private :
	DeviceManager() : app_(nullptr) {}

	AppBase* app_;
};
