#pragma once

#include "SceneBase.h"
#include <string>

/*!
 * @brief シーンの初期化パラメータ
 */
struct SceneGameParam : public SceneBaseParam
{
	std::string str;
};

/*!
 * @brief 初期化シーン
 */
class SceneGame : public SceneBase
{
public :
	~SceneGame(){}

	/*!
	 * @brief 初期化
	 */
	void Initialize(const SceneBaseParam* param);

	/*!
	 * @brief 終了化
	 */
	void Finalize();

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

private :
	SceneGameParam param_;
};
