/*!
 * @file
 *
 * @brief シーン管理クラスを提供します。
 *
 * @copyright
 */
#pragma once

#include "StateManager.h"
#include "scene/SceneBase.h"
#include "scene/SceneList.h"
#include <memory>
#include <stack>

/*!
 * @brief シーン管理クラス
 */
class SceneManager
{
public :
	/*!
	 * @brief シーンを切り替える
	 *
	 * @param scene 切り替えるシーン
	 * @param param 渡すパラメータ
	 */
	void Change(SceneList::List scene, std::shared_ptr<SceneBaseParam> param);

	/*!
	 * @brief 前のシーンに戻る
	 *
	 * @return 前のシーンがない場合は false を返します。
	 */
	bool Back();

	/*!
	 * @brief シーンのスタックをクリアする
	 */
	void Clear();

	/*!
	 * @brief シーンを最初から処理する
	 */
	void Restart();

	/*!
	 * @brief 更新
	 *
	 * @param df 前のフレームとの差分時間
	 */
	void Update(float df);

	/*!
	* @brief 描画
	*/
	void Render();

	static SceneManager* GetInstance() {
		static SceneManager v;
		return &v;
	}
private :
	SceneManager() : last_scene_(SceneList::None), is_restart_(false) {}

	struct StackInfo {
		SceneList::List last_scene_;
		std::shared_ptr<SceneBaseParam> last_param_;
	};

	StateManager<SceneList::List> scene_state_;
	std::shared_ptr<SceneBase> scene_;
	SceneList::List last_scene_;
	std::shared_ptr<SceneBaseParam> last_param_;
	std::stack<StackInfo> stack_;

	bool is_restart_;
};
