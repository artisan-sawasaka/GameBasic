/*!
 * @file
 *
 * @brief フェード管理クラスを提供します。
 *
 * @copyright
 */
#pragma once

#include "bezier.h"
#include "render/Color.h"
#include <Windows.h>

/*!
 * @brief フェード管理クラスを提供します。
 */
class FadeManager
{
public :
	/*!
	 * @brief フェードイン
	 */
	void FadeIn(float time = 0.3f, const Color& color = Color::Black, float delay = 0.0f);

	/*!
	 * @brief フェードアウト
	 */
	void FadeOut(float time = 0.3f, const Color& color = Color::Black, float delay = 0.0f);

	/*!
	 * @brief 終了判定
	 */
	bool IsEnd() const { return delay_time_ <= 0 && alpha_.IsEnd(); }

	/*!
	 * @brief 更新
	 */
	void Update(float df);

	/*!
	 * @brief 描画
	 */
	void Render();

	static FadeManager* GetInstance() {
		static FadeManager v;
		return &v;
	}

private :
	FadeManager() : delay_time_(0.0f), color_(Color::Black) { alpha_.Set(255, 255, 0.0f, Bezier::Linear); }

	float delay_time_;
	Color color_;
	Bezier::Timer<int> alpha_;
};
