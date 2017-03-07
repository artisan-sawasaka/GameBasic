#pragma once

#include "SceneBase.h"
#include "render/Texture.h"
#include "utility/StateManager.h"
#include "utility/InOutAnimation.h"
#include "master/MasterData.hpp"
#include "3d/SkinModel.h"
#include "render_utility/RasterScroll.h"
#include "shader/BlurShader.h"
#include <memory>
#include <map>

/*!
 * @brief タイトルシーン
 */
class SceneTitle : public SceneBase
{
public :
	~SceneTitle(){}

	/*!
	 * @brief 初期化
	 */
	void Initialize(const SceneBaseParam* param);

	/*!
	 * @brief 更新
	 *
	 * @param df 前のフレームとの差分時間
	 */
	void Update(float df);

	/*!
	 * @brief 3D描画
	 */
	void Render3D();

	/*!
	 * @brief 2D描画
	 */
	void Render2D();

private :
	enum State {
		ST_INIT,
		ST_LOADING,

		// インアニメーション
		ST_IN_ANIMATION_INIT,
		ST_IN_ANIMATION,

		// 更新
		ST_SELECT_INIT,
		ST_SELECT,

		// アウトアニメーション
		ST_OUT_ANIMATION_INIT,
		ST_OUT_ANIMATION,

		// ゲーム選択時のアウトアニメーション
		ST_START_GAME_ANIMATION_INIT,
		ST_START_GAME_ANIMATION,

		ST_EXIT,
	};
	
	enum Menu {
		Start,
		Option,
		Exit,

		MenuMax,
	};

	void Reload_();								// データ再読み込み
	void UpdateCursor_();						// カーソル更新

	// 処理
	bool ActionInAnimation_(float df);			// インアニメーション
	bool ActionSelect_(float df);				// 選択
	bool ActionOutAnimation_(float df);			// アウトアニメーション
	bool ActionStartGameAnimation_(float df);	// アウトアニメーション

	void Render2D_();
	void Debug_();

	std::map<std::string, std::shared_ptr<Texture>> textures_;						// 画像データ
	std::vector<MasterData::TitleUIData> ui_;										// UI
	std::map<std::string, MasterData::TitleUIData*> objects_;						// オブジェクト
	InOutAnimation<MasterData::TitleUIData, MasterData::TitleInOutData> animtion_;	// InOutアニメーション
	StateManager<State> state_;														// 状態管理

	//Model model_;
	SkinModel model_;
	Menu cursor_;	// カーソル

	Texture render_target_;
	RasterScroll raster_scroll_;
	BlurShader shader_;
};
