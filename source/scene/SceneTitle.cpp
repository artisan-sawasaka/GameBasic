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

	cursor_ = 0;

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
		animtion_.SetIn(MasterData::TitleUI, MasterData::TitleInOut);
		state_.Change(ST_FADE_IN_INIT, true);
	}

	// フェードイン
	if (state_.IsRange(ST_FADE_IN_INIT, ST_FADE_IN)) {
		if (ActionFadeIn_(df)) {
			state_.Change(ST_SELECT_INIT);
		}
	}

	// 更新
	if (state_.IsRange(ST_SELECT_INIT, ST_SELECT)) {
		if (ActionSelect_(df)) {
			state_.Change(ST_FADE_OUT_INIT);
		}
	}

	// フェードアウト
	if (state_.IsRange(ST_FADE_OUT_INIT, ST_FADE_OUT)) {
		if (ActionFadeOut_(df)) {
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

	objects_["StartButton"]->str = button[cursor_ == 0 ? 1 : 0];
	objects_["OptionButton"]->str = button[cursor_ == 1 ? 1 : 0];
	objects_["ExitButton"]->str = button[cursor_ == 2 ? 1 : 0];
}

bool SceneTitle::ActionFadeIn_(float df)
{
	if (state_ == ST_FADE_IN_INIT) {
		FadeManager::GetInstance()->FadeIn();
		state_.Change(ST_FADE_IN, true);
	}
	if (state_ == ST_FADE_IN) {
		animtion_.Update(df);
		if (FadeManager::GetInstance()->IsEnd() && animtion_.IsEnd()) {
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
			cursor_ = (cursor_ + 2) % 3;
			SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
		} else if (KeyManager::GetInstance()->IsTrg(VK_DOWN)) {
			// 下
			cursor_ = ++cursor_ % 3;
			SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
		} else if (KeyManager::GetInstance()->IsTrg(VK_RETURN)) {
			// 決定
			SoundManager::GetInstance()->PlaySe(CRI_SE_OK, 0);
			if (cursor_ == 0) {
				// 開始
			} else if (cursor_ == 0) {
				// オプション
			} else {
				// 終了
				return true;
			}
		}
	}

	return false;
}

bool SceneTitle::ActionFadeOut_(float df)
{
	if (state_ == ST_FADE_OUT_INIT) {
		animtion_.SetOut(MasterData::TitleUI, MasterData::TitleInOut);
		FadeManager::GetInstance()->FadeOut(0.3f, Gdiplus::Color::Black, 0.2f);
		state_.Change(ST_FADE_OUT, true);
	}
	if (state_ == ST_FADE_OUT) {
		animtion_.Update(df);
		if (FadeManager::GetInstance()->IsEnd() && animtion_.IsEnd()) {
			DeviceManager::GetInstance()->Exit();
			return true;
		}
	}

	return false;
}
