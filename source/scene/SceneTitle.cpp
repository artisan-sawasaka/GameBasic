#include "SceneTitle.h"
#include "utility/Renderer.h"
#include "utility/Utility.hpp"
#include "utility/DeviceManager.h"
#include "utility/FadeManager.h"
#include "utility/KeyManager.h"
#include "utility/SoundManager.h"
#include "sound/Bgm.h"
#include "sound/Se.h"

/*!
 * @brief 更新
 */
void SceneTitle::Initialize(const SceneBaseParam* param)
{
	// マスターデータ読み込み
	Reload_();
	
	// 各種カウンター初期化
	stop_counter_.set(0, DeviceManager::GetInstance()->GetWidth() - 150, 6);
	root_counter_.set(0, DeviceManager::GetInstance()->GetWidth(), 4);
	cursor_counter_.set(0, 10, -3);
	SetBezier_(Bezier::Linear);

	state_.Change(ST_INIT);
}

/*!
 * @brief 更新
 */
void SceneTitle::Update(float df)
{
	state_.Check(df);
	// 初期化
	if (state_ == ST_INIT) {
		state_.Change(ST_FADE_IN_INIT, true);
	}

	// フェードイン
	if (state_ == ST_FADE_IN_INIT) {
		FadeManager::GetInstance()->FadeIn();
		state_.Change(ST_FADE_IN, true);
	}
	if (state_ == ST_FADE_IN) {
		if (FadeManager::GetInstance()->IsEnd()) {
			state_.Change(ST_UPDATE);
		}
	}

	if (GetKeyState('S') < 0) {
		// 止まったカウンターを再度再生
		stop_counter_.set(0, stop_counter_.getEnd(), 6);
	} else if (KeyManager::GetInstance()->IsTrg('1')) {
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_VILLAGE);
	} else if (KeyManager::GetInstance()->IsTrg('2')) {
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_FIELD);
	} else if (KeyManager::GetInstance()->IsTrg('3')) {
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_BATTLE);
	} else if (KeyManager::GetInstance()->IsTrg('4')) {
		SoundManager::GetInstance()->PlaySe(CRI_SE_OK, 0);
	} else if (KeyManager::GetInstance()->IsTrg('5')) {
		SoundManager::GetInstance()->PlaySe(CRI_SE_CANCEL, 0);
	} else if (KeyManager::GetInstance()->IsTrg('6')) {
		SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
	}

	++stop_counter_;
	++root_counter_;
	++cursor_counter_;
	++bezier_counter_;
	bezier_timer_.Update(df);

	// とまる～を操作
	objects_["TextStop"]->x = stop_counter_;

	// まわる～を操作
	objects_["TextLoop"]->x = root_counter_;

	// カーソルを操作
	objects_["TextCursor"]->y = cursour_y_ + cursor_counter_;

	// ベジェカウンターを操作
	objects_["BezierCounter"]->x = bezier_counter_;

	// カーソルを操作
	objects_["BezierTimer"]->x = bezier_timer_;
}

/*!
 * @brief 描画
 */
void SceneTitle::Render()
{
	// マスターデータに基づく描画処理
	Utility::BasicRender(MasterData::TitleImageList, MasterData::TitleUI, bitmaps_);
}

/*!
 * @brief マスター再読み込み
 */
void SceneTitle::Reload_()
{
	// マスターデータを再読み込み
	Utility::ReloadMasterData();

	// FPSの設定
	DeviceManager::GetInstance()->SetFPS(MasterData::Const.FPS);

	// 画像を読み込み
	bitmaps_ = Utility::CreateBitmaps(MasterData::TitleImageList);

	// 操作しやすいようにオブジェクト化する
	objects_ = Utility::CreateObjects<MasterData::TitleUIData>(MasterData::TitleUI);

	cursour_y_ = objects_["TextCursor"]->y;
}

void SceneTitle::SetBezier_(const Bezier::ControlPoint& cp)
{
	bezier_counter_.Set(100, 400, 60, cp);
	bezier_timer_.Set(100, 400, 1.0f, cp);
}
