#include "SceneTitle.h"
#include "utility/Renderer.h"
#include "utility/Utility.hpp"
#include "utility/DeviceManager.h"
#include "utility/FadeManager.h"
#include "utility/KeyManager.h"

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
	fade_counter_.set(0, 0, 1);
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

	// フェード
	if (state_ == ST_FADE_IN_INIT) {
		FadeManager::GetInstance()->FadeIn();
		state_.Change(ST_FADE_IN, true);
	}
	if (state_ == ST_FADE_IN) {
		if (FadeManager::GetInstance()->IsEnd()) {
			state_.Change(ST_UPDATE, true);
		}
	}

	if (GetKeyState('R') < 0) {
		// マスターデータ再読み込み
		Reload_();
	} else if (GetKeyState('I') < 0) {
		// フェードイン
		fade_counter_.set(fade_counter_, 0, 30);
	} else if (GetKeyState('O') < 0) {
		// フェードアウト
		fade_counter_.set(fade_counter_, 255, 30);
	} else if (GetKeyState('S') < 0) {
		// 止まったカウンターを再度再生
		stop_counter_.set(0, stop_counter_.getEnd(), 6);
	} else if (KeyManager::GetInstance()->IsPress('1')) {
		SetBezier_(Bezier::Linear);
	} else if (GetKeyState('2') < 0) {
		SetBezier_(Bezier::EaseIn);
	} else if (GetKeyState('3') < 0) {
		SetBezier_(Bezier::EaseOut);
	} else if (GetKeyState('4') < 0) {
		SetBezier_(Bezier::EaseInOut);
	} else if (GetKeyState('5') < 0) {
		SetBezier_(Bezier::EaseInBack);
	} else if (GetKeyState('6') < 0) {
		SetBezier_(Bezier::EaseOutBack);
	} else if (GetKeyState('7') < 0) {
		SetBezier_(Bezier::EaseInOutBack);
	} else if (GetKeyState('8') < 0) {
		SetBezier_(Bezier::EaseInCirc);
	} else if (GetKeyState('9') < 0) {
		SetBezier_(Bezier::ControlPoint(MasterData::Const.x1, MasterData::Const.y1, MasterData::Const.x2, MasterData::Const.y2));
	}

	++stop_counter_;
	++root_counter_;
	++fade_counter_;
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
	Utility::BaseRender(MasterData::TitleImageList, MasterData::TitleUI, bitmaps_);

	// 操作説明
	static const char* ds[] = {
		"R:マスター読み込み",
		"I:フェードイン",
		"O:フェードアウト",
		"S:とまる～を再度移動",
		"1:Bezier::Linear:",
		"2:Bezier::EaseIn:",
		"3:Bezier::EaseOut",
		"4:Bezier::EaseInOut",
		"5:Bezier::EaseInBack",
		"6:Bezier::EaseOutBack",
		"7:Bezier::EaseInOutBack",
		"8:Bezier::EaseInCirc",
		"9:Bezier オリジナル",
	};
	for (int i = 0; i < sizeof(ds) / sizeof(*ds); ++i) {
		Renderer::GetInstance()->DrawString(ds[i], Renderer::LEFT_TOP, 0, i * 16, 12);
	}
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

	// 操作しやすいようにデータ化する
	objects_ = Utility::CreateObjects<MasterData::TitleUIData>(MasterData::TitleUI);

	cursour_y_ = objects_["TextCursor"]->y;
}

void SceneTitle::SetBezier_(const Bezier::ControlPoint& cp)
{
	bezier_counter_.Set(100, 400, 60, cp);
	bezier_timer_.Set(100, 400, 1.0f, cp);
}
