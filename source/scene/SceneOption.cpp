#include "SceneOption.h"
#include "render/Renderer.h"
#include "input/Key.h"
#include "utility/Utility.hpp"
#include "utility/FadeManager.h"
#include "scene/SceneManager.h"
#include "sound/SoundManager.h"
#include "sound/Se.h"

static const int MaxVolume = 10;

/*!
 * @brief 更新
 */
void SceneOption::Initialize(const SceneBaseParam* param)
{
	state_.Change(ST_INIT);
}

/*!
 * @brief 更新
 */
void SceneOption::Update(float df)
{
	state_.Update(df);

	// 初期化
	if (state_ == ST_INIT) {
		state_.Change(ST_LOADING, true);
	}
	if (state_ == ST_LOADING) {
		Reload_();
		state_.Change(ST_IN_ANIMATION_INIT);
	}

	// インアニメーション
	if (state_.IsRange(ST_IN_ANIMATION_INIT, ST_IN_ANIMATION)) {
		if (ActionInAnimation_(df)) {
			state_.Change(ST_SELECT_INIT);
		}
	}

	// 選択
	if (state_.IsRange(ST_SELECT_INIT, ST_SELECT)) {
		if (ActionSelect_(df)) {
			state_.Change(ST_OUT_ANIMATION_INIT);
		}
	}

	// アウトアニメーション
	if (state_.IsRange(ST_OUT_ANIMATION_INIT, ST_OUT_ANIMATION)) {
		if (ActionOutAnimation_(df)) {
			state_.Change(ST_SELECT_INIT);
		}
	}

	// カーソル更新
	UpdateCursor_();
}

/*!
 * @brief 描画
 */
void SceneOption::Render2D()
{
	// マスターデータに基づく描画処理
	Utility::BasicRender(MasterData::OptionImageList, ui_, textures_);
}

/*!
 * @brief データ再読み込み
 */
void SceneOption::Reload_()
{
	// 画像を読み込み
	textures_ = Utility::CreateBitmaps(MasterData::OptionImageList);

	// UIをコピーして保持
	ui_ = MasterData::OptionUI;

	// 操作しやすいようにオブジェクト化する
	objects_ = Utility::CreateObjects<MasterData::OptionUIData>(ui_);

	// カーソルの初期位置を保持
	cursor_x_[BgmLeft] = objects_["BgmLeftArrow"]->x;
	cursor_x_[BgmRight] = objects_["BgmRightArrow"]->x;
	cursor_x_[SeLeft] = objects_["SeLeftArrow"]->x;
	cursor_x_[SeRight] = objects_["SeRightArrow"]->x;
	cursor_x_[VoiceLeft] = objects_["VoiceLeftArrow"]->x;
	cursor_x_[VoiceRight] = objects_["VoiceRightArrow"]->x;
	cursor_ = Bgm;
	SetCursor_();

	// 音量を取得
	volume_[Bgm] = static_cast<int>(SoundManager::GetInstance()->GetVolumeBgm() * 10);
	volume_[Se] = static_cast<int>(SoundManager::GetInstance()->GetVolumeSe() * 10);
	volume_[Voice] = static_cast<int>(SoundManager::GetInstance()->GetVolumeVoice() * 10);
	UpdateSound_();
}

/*!
 * @brief カーソルの設定
 */
void SceneOption::SetCursor_()
{
	bgm_cursor_move_.set(0, GetCursorMove(Bgm), GetCursorSpeed(Bgm));
	se_cursor_move_.set(0, GetCursorMove(Se), GetCursorSpeed(Se));
	voice_cursor_move_.set(0, GetCursorMove(Voice), GetCursorSpeed(Voice));
}

/*!
 * @brief カーソル更新
 */
void SceneOption::UpdateCursor_()
{
	// 選択項目の色変更
	Utility::SetObjectColor(objects_["BgmValue"], (cursor_ == Bgm) ? Color::Red : Color::White);
	Utility::SetObjectColor(objects_["SeValue"], (cursor_ == Se) ? Color::Red : Color::White);
	Utility::SetObjectColor(objects_["VoiceValue"], (cursor_ == Voice) ? Color::Red : Color::White);
	Utility::SetObjectColor(objects_["Exit"], (cursor_ == Exit) ? Color::Red : Color::White);

	// 左右矢印の移動
	if (state_ == ST_SELECT) {
		bgm_cursor_move_++;
		se_cursor_move_++;
		voice_cursor_move_++;
		objects_["BgmLeftArrow"]->x = cursor_x_[BgmLeft] - bgm_cursor_move_;
		objects_["BgmRightArrow"]->x = cursor_x_[BgmRight] + bgm_cursor_move_;
		objects_["SeLeftArrow"]->x = cursor_x_[SeLeft] - se_cursor_move_;
		objects_["SeRightArrow"]->x = cursor_x_[SeRight] + se_cursor_move_;
		objects_["VoiceLeftArrow"]->x = cursor_x_[VoiceLeft] - voice_cursor_move_;
		objects_["VoiceRightArrow"]->x = cursor_x_[VoiceRight] + voice_cursor_move_;
	}
}

/*!
 * @brief サウンド更新
 */
void SceneOption::UpdateSound_()
{
	// 音量の設定
	SoundManager::GetInstance()->SetVolumeBgm(volume_[Bgm] / 10.0f);
	SoundManager::GetInstance()->SetVolumeSe(volume_[Se] / 10.0f);
	SoundManager::GetInstance()->SetVolumeVoice(volume_[Voice] / 10.0f);

	// 音量表示の設定
	objects_["BgmValue"]->str = Utility::StringFormat("%3d%%", volume_[Bgm] * 10);
	objects_["SeValue"]->str = Utility::StringFormat("%3d%%", volume_[Se] * 10);
	objects_["VoiceValue"]->str = Utility::StringFormat("%3d%%", volume_[Voice] * 10);

	// 矢印の表示の有無
	objects_["BgmLeftArrow"]->visible =  (volume_[Bgm] != 0);
	objects_["BgmRightArrow"]->visible = (volume_[Bgm] != MaxVolume);
	objects_["SeLeftArrow"]->visible = (volume_[Se] != 0);
	objects_["SeRightArrow"]->visible = (volume_[Se] != MaxVolume);
	objects_["VoiceLeftArrow"]->visible = (volume_[Voice] != 0);
	objects_["VoiceRightArrow"]->visible = (volume_[Voice] != MaxVolume);
}

/*!
 * @brief カーソル移動量取得
 */
int SceneOption::GetCursorMove(Menu menu) const
{
	return cursor_ == menu ? MasterData::OptionConst.CursorMove : 0;
}

/*!
 * @brief カーソル移動速度取得
 */
int SceneOption::GetCursorSpeed(Menu menu) const
{
	return cursor_ == menu ? MasterData::OptionConst.CursorSpeed : 0;
}

/*!
 * @brief インアニメーション
 */
bool SceneOption::ActionInAnimation_(float df)
{
	if (state_ == ST_IN_ANIMATION_INIT) {
		animtion_.SetIn(ui_, MasterData::OptionInOut);
		state_.Change(ST_IN_ANIMATION, true);
	}
	if (state_ == ST_IN_ANIMATION) {
		animtion_.Update(df);
		if (animtion_.IsEnd()) {
			return true;
		}
	}

	return false;
}

/*!
 * @brief 選択
 */
bool SceneOption::ActionSelect_(float df)
{
	if (state_ == ST_SELECT_INIT) {
		state_.Change(ST_SELECT, true);
	}
	if (state_ == ST_SELECT) {
		if (Key::GetInstance()->IsTrg(KeyCode::UP)) {
			// 上
			cursor_ = static_cast<Menu>((cursor_ + (MenuMax - 1)) % MenuMax);
			SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
			SetCursor_();
		} else if (Key::GetInstance()->IsTrg(KeyCode::DOWN)) {
			// 下
			cursor_ = static_cast<Menu>((cursor_ + 1) % MenuMax);
			SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
			SetCursor_();
		} else if (Key::GetInstance()->IsRepeat(KeyCode::LEFT) && cursor_ != Exit) {
			// 左
			if (volume_[cursor_] > 0) {
				SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
				volume_[cursor_]--;
				UpdateSound_();
			}
		} else if (Key::GetInstance()->IsRepeat(KeyCode::RIGHT) && cursor_ != Exit) {
			// 右
			if (volume_[cursor_] < MaxVolume) {
				SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
				volume_[cursor_]++;
				UpdateSound_();
			}
		} else if (Key::GetInstance()->IsTrg(KeyCode::ENTER) && cursor_ == Exit) {
			// 終了
			SoundManager::GetInstance()->PlaySe(CRI_SE_OK, 0);
			return true;
		}
	}

	return false;
}

/*!
 * @brief アウトアニメーション
 */
bool SceneOption::ActionOutAnimation_(float df)
{
	if (state_ == ST_OUT_ANIMATION_INIT) {
		animtion_.SetOut(ui_, MasterData::OptionInOut);
		state_.Change(ST_OUT_ANIMATION, true);
	}
	if (state_ == ST_OUT_ANIMATION) {
		animtion_.Update(df);
		if (animtion_.IsEnd()) {
			SceneManager::GetInstance()->Back();
			return true;
		}
	}

	return false;
}
