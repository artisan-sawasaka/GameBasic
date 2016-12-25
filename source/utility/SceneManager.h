#pragma once

#include "StateManager.h"
#include "scene/SceneBase.h"
#include "scene/SceneList.h"
#include <memory>

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
	 * @brief シーンを最初から処理する
	 */
	void Restart();

	/*!
	 * @brief 更新
	 *
	 * @param df 前のフレームとの差分時間
	 */
	void Update(float df);

	static SceneManager* GetInstance()
	{
		static SceneManager v;
		return &v;
	}
private :
	SceneManager() : is_restart_(false) {}

	StateManager<SceneList::List> scene_state_;
	std::shared_ptr<SceneBase> scene_;
	std::shared_ptr<SceneBaseParam> param_;
	bool is_restart_;
};
