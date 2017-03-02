#pragma once

#include "SceneBase.h"
#include "utility/counter.h"
#include "utility/StateManager.h"
#include "utility/InOutAnimation.h"
#include "master/MasterData.hpp"
#include "render/Texture.h"
#include <memory>
#include <map>

/*!
 * @brief オプションシーン
 */
class SceneOption : public SceneBase
{
public :
	~SceneOption(){}

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
	 * @brief 描画
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

		ST_EXIT,
	};
	
	enum Menu {
		Bgm,
		Se,
		Voice,
		Exit,

		MenuMax,
	};

	enum Cursor {
		BgmLeft,
		BgmRight,
		SeLeft,
		SeRight,
		VoiceLeft,
		VoiceRight,

		CursorMax,
	};

	void Reload_();							// データ再読み込み
	void SetCursor_();						// カーソルの設定
	void UpdateCursor_();					// カーソル更新
	void UpdateSound_();					// サウンド更新
	int GetCursorMove(Menu menu) const;		// カーソル移動量取得
	int GetCursorSpeed(Menu menu) const;	// カーソル移動速度取得

	// 処理
	bool ActionInAnimation_(float df);		// インアニメーション
	bool ActionSelect_(float df);			// 選択
	bool ActionOutAnimation_(float df);		// アウトアニメーション

	std::map<std::string, std::shared_ptr<Texture>> textures_;							// 画像データ
	std::vector<MasterData::OptionUIData> ui_;											// UI
	std::map<std::string, MasterData::OptionUIData*> objects_;							// オブジェクト
	InOutAnimation<MasterData::OptionUIData, MasterData::OptionInOutData> animtion_;	// InOutアニメーション
	StateManager<State> state_;															// 状態管理

	Menu cursor_;
	LoopCounter bgm_cursor_move_;
	LoopCounter se_cursor_move_;
	LoopCounter voice_cursor_move_;
	int cursor_x_[CursorMax];
	int volume_[CursorMax - 1];
};
