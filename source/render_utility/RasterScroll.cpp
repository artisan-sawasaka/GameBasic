#include "RasterScroll.h"
#include "render/Renderer.h"

RasterScroll::RasterScroll()
	: end_(true)
{
	Set();
	SetRate();
	color_ = Color::White;
}

RasterScroll::~RasterScroll()
{
}

void RasterScroll::Set(float offset, float period, float amplitude)
{
	shader_.SetOffset(offset);
	shader_.SetPeriod(period);
	shader_.SetAmplitude(amplitude);
}

void RasterScroll::SetRate(float offset, float period, float amplitude)
{
	offset_rate_ = offset;
	period_rate_ = period;
	amplitude_rate_ = amplitude;
}

void RasterScroll::Start(float scroll_time, float fade_time)
{
	scroll_time_ = scroll_time;
	fade_time_ = fade_time;
	time_ = 0;
	color_ = Color::White;

	end_ = false;
}

void RasterScroll::Update(float df)
{
	if (end_) return ;

	shader_.SetOffset(shader_.GetOffset() + df * offset_rate_);
	shader_.SetPeriod(shader_.GetPeriod() + df * period_rate_);
	shader_.SetAmplitude(shader_.GetAmplitude() + df * amplitude_rate_);
	time_ += df;

	if (time_ >= scroll_time_ + fade_time_) {
		end_ = true;
		//color_ = Color::Black;
	} else if (time_ >= scroll_time_) {
		float time = time_ - scroll_time_;
		uint8_t v = static_cast<uint8_t>((1.0f - time / fade_time_) * 255.0f);
		color_.SetA(v);
	}
}

void RasterScroll::Render(Texture* texture)
{
	shader_.Begin();
	shader_.SetTexture(texture);
	shader_.SetColor(color_);
	Renderer::GetInstance()->DrawImage(texture, Renderer::LEFT_TOP, 0, 0, color_);
	shader_.End();
}
