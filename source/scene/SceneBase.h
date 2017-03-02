#pragma once

/*!
 * @brief シーンの初期化パラメータの基底クラス
 */
struct SceneBaseParam
{
public :
	virtual ~SceneBaseParam(){}
};

/*!
 * @brief シーンの基底クラス
 */
class SceneBase
{
public :
	virtual ~SceneBase(){}

	/*!
	 * @brief 初期化
	 */
	virtual void Initialize(const SceneBaseParam* param){}

	/*!
	 * @brief 終了化
	 */
	virtual void Finalize(){}

	/*!
	 * @brief 更新
	 *
	 * @param df 前のフレームとの差分時間
	 */
	virtual void Update(float df){}

	/*!
	 * @brief 3D描画
	 */
	virtual void Render3D(){}

	/*!
	 * @brief 2D描画
	 */
	virtual void Render2D(){}
};
