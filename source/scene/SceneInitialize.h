#pragma once

#include "SceneBase.h"

/*!
 * @brief 初期化シーン
 */
class SceneInitialize : public SceneBase
{
public :
	~SceneInitialize(){}

	/*!
	 * @brief 更新
	 *
	 * @param df 前のフレームとの差分時間
	 */
	void Update(float df);

	/*!
	 * @brief 描画
	 */
	void Render2D();
};
