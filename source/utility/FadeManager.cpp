#include "FadeManager.h"
#include "Renderer.h"
#include "DeviceManager.h"
#include "ConsoleManager.h"

/*!
 * @brief フェードイン
 */
void FadeManager::FadeIn(float time, const Gdiplus::Color& color, float delay)
{
	alpha_.Set(255, 0, time, Bezier::Linear);
	delay_time_ = delay;
}

/*!
 * @brief フェードアウト
 */
void FadeManager::FadeOut(float time, const Gdiplus::Color& color, float delay)
{
	alpha_.Set(0, 255, time, Bezier::Linear);
	delay_time_ = delay;
}

/*!
 * @brief 更新
 */
void FadeManager::Update(float df)
{
	if (delay_time_ > 0) {
		delay_time_ -= df;
		ConsoleManager::GetInstance()->Print("time:%9.3f\n", delay_time_);
		if (delay_time_ > 0) return;
	}
	alpha_.Update(df);
}

/*!
 * @brief 描画
 */
void FadeManager::Render()
{
	if (alpha_ == 0) return ;

	ConsoleManager::GetInstance()->Print("alpha:%d\n", alpha_.Get());
	color_.SetValue((color_.GetValue() & ~Gdiplus::Color::AlphaMask) | ((alpha_ & 0xff) << Gdiplus::Color::AlphaShift));
	Renderer::GetInstance()->FillRect(0, 0, DeviceManager::GetInstance()->GetWidth(), DeviceManager::GetInstance()->GetHeight(), color_);
}
