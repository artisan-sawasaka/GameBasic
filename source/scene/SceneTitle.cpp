#include "SceneTitle.h"
#include "utility/Renderer.h"
#include "utility/Utility.hpp"
#include "utility/DeviceManager.h"
#include "utility/FadeManager.h"
#include "utility/KeyManager.h"
#include "utility/SoundManager.h"
#include "utility/SceneManager.h"
#include "sound/Bgm.h"
#include "sound/Se.h"

/*!
 * @brief 更新
 */
void SceneTitle::Initialize(const SceneBaseParam* param)
{
	// マスターデータ読み込み
	cursor_ = Start;

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
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_VILLAGE);
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

	// 更新
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

	// カーソルチェック
	CheckCursor_();

	if (KeyManager::GetInstance()->IsTrg('1')) {
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
	} else if (KeyManager::GetInstance()->IsTrg('9')) {
		SoundManager::GetInstance()->StopAll();
	}
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
}

void SceneTitle::CheckCursor_()
{
	static const char* button[] = { "ButtonGray", "ButtonRed" };

	objects_["StartButton"]->str = button[cursor_ == Start ? 1 : 0];
	objects_["OptionButton"]->str = button[cursor_ == Option ? 1 : 0];
	objects_["ExitButton"]->str = button[cursor_ == Exit ? 1 : 0];
}

bool SceneTitle::ActionInAnimation_(float df)
{
	if (state_ == ST_IN_ANIMATION_INIT) {
		animtion_.SetIn(MasterData::TitleUI, MasterData::TitleInOut);
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

bool SceneTitle::ActionSelect_(float df)
{
	if (state_ == ST_SELECT_INIT) {
		state_.Change(ST_SELECT, true);
	}
	if (state_ == ST_SELECT) {
		if (KeyManager::GetInstance()->IsTrg(VK_UP)) {
			// 上
			cursor_ = static_cast<Menu>((cursor_ + (MenuMax - 1)) % MenuMax);
			SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
		} else if (KeyManager::GetInstance()->IsTrg(VK_DOWN)) {
			// 下
			cursor_ = static_cast<Menu>((cursor_ + 1) % MenuMax);
			SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
		} else if (KeyManager::GetInstance()->IsTrg(VK_RETURN)) {
			// 決定
			SoundManager::GetInstance()->PlaySe(CRI_SE_OK, 0);
			return true;
		}
	}

	return false;
}

bool SceneTitle::ActionOutAnimation_(float df)
{
	if (state_ == ST_OUT_ANIMATION_INIT) {
		animtion_.SetOut(MasterData::TitleUI, MasterData::TitleInOut);
		state_.Change(ST_OUT_ANIMATION, true);
	}
	if (state_ == ST_OUT_ANIMATION) {
		animtion_.Update(df);
		if (animtion_.IsEnd()) {
			if (cursor_ == Exit) {
				DeviceManager::GetInstance()->Exit();
			}
			SceneManager::GetInstance()->Restart();
			return true;
		}
	}

	return false;
}
