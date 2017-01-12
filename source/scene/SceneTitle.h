#pragma once

#include "SceneBase.h"
#include "utility/StateManager.h"
#include "utility/InOutAnimation.h"
#include "master/MasterData.hpp"
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
	 * @brief 描画
	 */
	void Render();

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
		Start,
		Option,
		Exit,

		MenuMax,
	};

	void Reload_();							// データ再読み込み
	void UpdateCursor_();					// カーソル更新

	// 処理
	bool ActionInAnimation_(float df);		// インアニメーション
	bool ActionSelect_(float df);			// 選択
	bool ActionOutAnimation_(float df);		// アウトアニメーション


	std::map<std::string, std::shared_ptr<Gdiplus::Bitmap>> bitmaps_;				// 画像データ
	std::map<std::string, MasterData::TitleUIData*> objects_;						// 表示物
	InOutAnimation<MasterData::TitleUIData, MasterData::TitleInOutData> animtion_;	// InOutアニメーション
	StateManager<State> state_;														// 状態管理

	Menu cursor_;	// カーソル
};
