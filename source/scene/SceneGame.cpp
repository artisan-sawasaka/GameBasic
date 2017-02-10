#include "SceneGame.h"
#include "utility/SceneManager.h"
#include "utility/Renderer.h"
#include "utility/KeyManager.h"

/*!
 * @brief 初期化
 */
void SceneGame::Initialize(const SceneBaseParam* param)
{
	param_ = *(static_cast<const SceneGameParam*>(param));
}

/*!
 * @brief 終了化
 */
void SceneGame::Finalize()
{
}

/*!
 * @brief 更新
 */
void SceneGame::Update(float df)
{
	if (KeyManager::GetInstance()->IsTrg('A')) {
		SceneManager::GetInstance()->Back();
	}
}

/*!
 * @brief 描画
 */
void SceneGame::Render()
{
}
