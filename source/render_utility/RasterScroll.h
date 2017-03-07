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
	void Start(float scroll_time, float fade_time);
	void Start();
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
