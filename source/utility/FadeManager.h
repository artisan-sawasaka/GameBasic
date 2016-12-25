#pragma once

#include "bezier.h"
#include <Windows.h>
#include <gdiplus.h>

class FadeManager
{
public :
	/*!
	 * @brief フェードイン
	 */
	void FadeIn(float time = 0.3f, const Gdiplus::Color& color = Gdiplus::Color::Black);

	/*!
	 * @brief フェードアウト
	 */
	void FadeOut(float time = 0.3f, const Gdiplus::Color& color = Gdiplus::Color::Black);

	/*!
	 * @brief 終了判定
	 */
	bool IsEnd() const { return alpha_.IsEnd(); }

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
	FadeManager(){}

	Bezier::Timer<int> alpha_;
	Gdiplus::Color color_;
};
