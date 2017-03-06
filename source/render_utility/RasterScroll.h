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

	void Set(float offset = 0.0f, float period = 0.0f, float amplitude = 0.1f);
	void SetRate(float offset = 2.0f, float period = 1.0f, float amplitude = 0.1f);
	void Start(float scroll_time = 50.0f, float fade_time = 1.0f);
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

	Color color_;
};
