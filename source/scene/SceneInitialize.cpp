#include "SceneInitialize.h"
#include "utility/SceneManager.h"

/*!
 * @brief XV
 */
void SceneInitialize::Update(float df)
{
	SceneManager::GetInstance()->Change(SceneList::Title, nullptr);
}

/*!
 * @brief •`‰æ
 */
void SceneInitialize::Render()
{
}
