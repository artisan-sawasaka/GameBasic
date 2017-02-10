#include "SceneGame.h"
#include "utility/SceneManager.h"
#include "utility/Renderer.h"
#include "utility/KeyManager.h"

/*!
 * @brief ‰Šú‰»
 */
void SceneGame::Initialize(const SceneBaseParam* param)
{
	param_ = *(static_cast<const SceneGameParam*>(param));
}

/*!
 * @brief I—¹‰»
 */
void SceneGame::Finalize()
{
}

/*!
 * @brief XV
 */
void SceneGame::Update(float df)
{
	if (KeyManager::GetInstance()->IsTrg('A')) {
		SceneManager::GetInstance()->Back();
	}
}

/*!
 * @brief •`‰æ
 */
void SceneGame::Render()
{
}
