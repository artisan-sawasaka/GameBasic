#include "SceneGame.h"
#include "render/Renderer.h"
#include "input/Key.h"
#include "scene/SceneManager.h"
#include "utility/ConsoleManager.h"

/*!
 * @brief ‰Šú‰»
 */
void SceneGame::Initialize(const SceneBaseParam* param)
{
	param_ = *(static_cast<const SceneGameParam*>(param));
	ConsoleManager::GetInstance()->
		PrintLine(param_.str.c_str());
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
	if (Key::GetInstance()->IsTrg(KeyCode::BACK) || Key::GetInstance()->IsTrg(KeyCode::ENTER)) {
		SceneManager::GetInstance()->Back();
	}
}

/*!
 * @brief •`‰æ
 */
void SceneGame::Render2D()
{
}
