#include "RasterScroll.h"
#include "render/Renderer.h"
#include "master/MasterData.hpp"
#include "utility/Utility.hpp"

RasterScroll::RasterScroll()
	: end_(true)
{
	Set(MasterData::ConstRasterScroll.offset, MasterData::ConstRasterScroll.period, MasterData::ConstRasterScroll.amplitude);
	SetRate(MasterData::ConstRasterScroll.offset_rate, MasterData::ConstRasterScroll.period_rate, MasterData::ConstRasterScroll.amplitude_rate);
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

void RasterScroll::Start(float scroll_time, float fade_time, const Color& start_color, const Color& end_color)
{
	scroll_time_ = scroll_time;
	fade_time_ = fade_time;
	time_ = 0;
	start_color_ = start_color;
	end_color_ = end_color;

	end_ = false;
}

void RasterScroll::Start(const Color& start_color, const Color& end_color)
{
	Set(MasterData::ConstRasterScroll.offset, MasterData::ConstRasterScroll.period, MasterData::ConstRasterScroll.amplitude);
	SetRate(MasterData::ConstRasterScroll.offset_rate, MasterData::ConstRasterScroll.period_rate, MasterData::ConstRasterScroll.amplitude_rate);
	Start(MasterData::ConstRasterScroll.scroll_time, MasterData::ConstRasterScroll.fade_time, start_color, end_color);
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
		color_ = end_color_;
	} else if (time_ >= scroll_time_) {
		float rate = (time_ - scroll_time_) / fade_time_;
		color_.SetR(Utility::Lerp(start_color_.GetR(), end_color_.GetR(), rate));
		color_.SetG(Utility::Lerp(start_color_.GetG(), end_color_.GetG(), rate));
		color_.SetB(Utility::Lerp(start_color_.GetB(), end_color_.GetB(), rate));
		color_.SetA(Utility::Lerp(start_color_.GetA(), end_color_.GetA(), rate));
	}
}

void RasterScroll::Render(Texture* texture)
{
	if (end_) {
		Renderer::GetInstance()->DrawImage(texture, Renderer::LEFT_TOP, 0, 0, color_);
	} else {
		shader_.Begin();
		shader_.SetTexture(texture);
		shader_.SetColor(color_);
		Renderer::GetInstance()->DrawImage(texture, Renderer::LEFT_TOP, 0, 0, color_);
		shader_.End();
	}
}
