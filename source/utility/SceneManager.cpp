#include "SceneManager.h"
#include "scene/SceneBase.h"
#include "scene/SceneInitialize.h"
#include "scene/SceneTitle.h"

/*!
 * @brief シーン切り替え
 */
void SceneManager::Change(SceneList::List scene, std::shared_ptr<SceneBaseParam> param)
{
	if (last_scene_ != SceneList::None) {
		StackInfo info = {
			last_scene_,
			last_param_,
		};
		stack_.push(info);
	}

	last_scene_ = scene;
	last_param_ = param;
	scene_state_.Change(last_scene_);
}

/*!
 * @brief シーン切り替え
 */
bool SceneManager::Back()
{
	if (stack_.empty()) return false;

	last_scene_ = stack_.top().last_scene_;
	last_param_ = stack_.top().last_param_;
	stack_.pop();
	scene_state_.Change(last_scene_);

	return true;
}

/*!
 * @brief シーンのスタックをクリアする
 */
void SceneManager::Clear()
{
	std::stack<StackInfo>().swap(stack_);
}


/*!
 * @brief シーンを最初から処理する
 */
void SceneManager::Restart()
{
	is_restart_ = true;
}

/*!
 * @brief 更新
 */
void SceneManager::Update(float df)
{
	if (scene_state_.Update(df) || is_restart_) {
		is_restart_ = false;
		if (scene_) {
			scene_->Finalize();
		}
		switch (scene_state_) {
		case SceneList::Initialize :	scene_.reset(new SceneInitialize());	break;
		case SceneList::Title :			scene_.reset(new SceneTitle());			break;
		default : break;
		}

		if (scene_) {
			scene_->Initialize(last_param_.get());
		}
	}

	if (scene_) {
		scene_->Update(df);
	}
}

/*!
 * @brief 描画
 */
void SceneManager::Render()
{
	if (scene_) {
		scene_->Render();
	}
}
