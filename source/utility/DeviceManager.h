/*!
 * @file
 *
 * @brief デバイス管理クラスを提供します。
 *
 * @copyright
 */
#pragma once

#include "main/Device.h"

class AppBase;

/*!
 * @brief デバイス管理クラスを提供します。
 */
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

	/*!
	 * @brief 終了
	 */
	void Exit();

	/*!
	 * @brief デバイス取得
	 */
	Device* GetDevice();

	static DeviceManager* GetInstance()	{
		static DeviceManager v;
		return &v;
	}
private :
	DeviceManager() : app_(nullptr) {}

	AppBase* app_;
};
