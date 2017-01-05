#include "SceneInitialize.h"
#include "utility/SceneManager.h"
#include "utility/SoundManager.h"
#include "utility/Renderer.h"

/*!
 * @brief 更新
 */
void SceneInitialize::Update(float df)
{
	// サウンド初期化
	SoundManager::InitializeParam param {
		1,				//  BGMの最大同時再生数
		8,				//  SEの最大同時再生数
		4,				//  VOICEの最大同時再生数
		48000 * 2,		//  最大サンプリングレート
		60,				//  1FPS
		"data/sound/GameBasic.acf"		//  AFCファイルのパス
	};
	SoundManager::GetInstance()->Initialize(param);
	SoundManager::GetInstance()->LoadBgmFile("data/sound/Bgm.acb", "data/sound/Bgm.awb");	// BGM読み込み
	SoundManager::GetInstance()->LoadSeFile("data/sound/Se.acb");							// SE読み込み

	// タイトルへ遷移
	SceneManager::GetInstance()->Change(SceneList::Title, nullptr);
}

/*!
 * @brief 描画
 */
void SceneInitialize::Render()
{
}
