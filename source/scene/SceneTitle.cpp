#include "SceneTitle.h"
#include "render/Renderer.h"
#include "utility/Utility.hpp"
#include "utility/FadeManager.h"
#include "utility/KeyManager.h"
#include "utility/SceneManager.h"
#include "sound/SoundManager.h"
#include "sound/Bgm.h"
#include "sound/Se.h"
#include "scene/SceneGame.h"
#include <algorithm>

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
	state_.Update(df);

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

	if (KeyManager::GetInstance()->IsTrg('1')) {
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_VILLAGE);
	} else if (KeyManager::GetInstance()->IsTrg('2')) {
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_FIELD);
	} else if (KeyManager::GetInstance()->IsTrg('3')) {
		SoundManager::GetInstance()->PlayBgm(CRI_BGM_BATTLE);
	} else if (KeyManager::GetInstance()->IsTrg('4')) {
		SoundManager::GetInstance()->PlaySe(CRI_SE_OK);
	} else if (KeyManager::GetInstance()->IsTrg('5')) {
		SoundManager::GetInstance()->PlaySe(CRI_SE_CANCEL, 0);
	} else if (KeyManager::GetInstance()->IsTrg('6')) {
		SoundManager::GetInstance()->PlaySe(CRI_SE_CURSOR, 0);
	} else if (KeyManager::GetInstance()->IsTrg('9')) {
		SoundManager::GetInstance()->StopAll();
	}
}

/*!
 * @brief 3D描画
 */
void SceneTitle::Render3D()
{
	model_.Render();
}

/*!
 * @brief 描画
 */
void SceneTitle::Render2D()
{
	// マスターデータに基づく描画処理
	//Utility::BasicRender(MasterData::TitleImageList, ui_, textures_);
}

/*!
 * @brief データ再読み込み
 */
void SceneTitle::Reload_()
{
	// 画像を読み込み
	textures_ = Utility::CreateBitmaps(MasterData::TitleImageList);

	// UIをコピーして保持
	ui_ = MasterData::TitleUI;

	// 操作しやすいようにオブジェクト化する
	objects_ = Utility::CreateObjects<MasterData::TitleUIData>(ui_);

	model_.LoadFile("data/model/ch00_stand00.x");
	//model_.LoadFile("data/model/sample.x");
	
}

/*!
 * @brief カーソル更新
 */
void SceneTitle::UpdateCursor_()
{
	static const char* button[] = { "ButtonGray", "ButtonRed" };

	objects_["StartButton"]->str = button[cursor_ == Start ? 1 : 0];
	objects_["OptionButton"]->str = button[cursor_ == Option ? 1 : 0];
	objects_["ExitButton"]->str = button[cursor_ == Exit ? 1 : 0];
}

/*!
 * @brief インアニメーション
 */
bool SceneTitle::ActionInAnimation_(float df)
{
	if (state_ == ST_IN_ANIMATION_INIT) {
		animtion_.SetIn(ui_, MasterData::TitleInOut);
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
		} else if (KeyManager::GetInstance()->IsPress('Q')) {
			auto rotate = model_.GetRotate();
			rotate.x += 0.01f;
			model_.SetRotate(rotate);
		} else if (KeyManager::GetInstance()->IsPress('W')) {
			auto rotate = model_.GetRotate();
			rotate.y += 0.01f;
			model_.SetRotate(rotate);
		} else if (KeyManager::GetInstance()->IsPress('E')) {
			auto rotate = model_.GetRotate();
			rotate.z += 0.01f;
			model_.SetRotate(rotate);
		} else if (KeyManager::GetInstance()->IsPress('Z')) {
			model_.SetRotate(0, 0, 0);
		} else if (KeyManager::GetInstance()->IsPress('B')) {
			auto color = model_.GetColor();
			color.SetA(128);
			model_.SetColor(color);
		} else if (KeyManager::GetInstance()->IsPress('V')) {
			auto color = model_.GetColor();
			color.SetA(255);
			model_.SetColor(color);
		}
	}

	return false;
}

/*!
 * @brief アウトアニメーション
 */
bool SceneTitle::ActionOutAnimation_(float df)
{
	if (state_ == ST_OUT_ANIMATION_INIT) {
		animtion_.SetOut(ui_, MasterData::TitleInOut);
		state_.Change(ST_OUT_ANIMATION, true);
	}
	if (state_ == ST_OUT_ANIMATION) {
		animtion_.Update(df);
		if (animtion_.IsEnd()) {
			if (cursor_ == Start) {
				std::shared_ptr<SceneGameParam> param(new SceneGameParam());
				param->str = "あいうえお";
				SceneManager::GetInstance()->Change(SceneList::Game, param);
			} else if (cursor_ == Option) {
				SceneManager::GetInstance()->Change(SceneList::Option, nullptr);
			} else if (cursor_ == Exit) {
				DeviceManager::GetInstance()->Exit();
			}
			return true;
		}
	}

	return false;
}
