#pragma once

#include "shader/RasterScrollShader.h"
#include "render/Color.h"

/*!
 * @brief ラスタースクロール機能を追加
 */
class RasterScroll
{
public :
	RasterScroll();
	virtual ~RasterScroll();

	void Set(float offset, float period, float amplitude);
	void SetRate(float offset, float period, float amplitude);
	void Start(float scroll_time, float fade_time, const Color& start_color, const Color& end_color);
	void Start(const Color& start_color = Color::White, const Color& end_color = Color::Black);
	bool IsEnd() const { return end_; }

	void Update(float df);
	void Render(Texture* texture);

private :
	bool end_;
	RasterScrollShader shader_;

	float offset_rate_;
	float period_rate_;
	float amplitude_rate_;

	float time_;
	float scroll_time_;
	float fade_time_;

	Color start_color_;
	Color end_color_;
	Color color_;
};
