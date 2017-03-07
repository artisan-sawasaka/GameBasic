#pragma once

#include "Shader.h"
#include <string>

/*!
 * @brief ラスタースクロール
 */
class RasterScrollShader : public Shader
{
public :
	RasterScrollShader();
	~RasterScrollShader();

	virtual bool LoadFile(const char* path) { return false; }

	/*!
	 * @brief 縦の移動値を設定
	 */
	void SetOffset(float value);
	/*!
	 * @brief 横の移動値を設定
	 */
	void SetPeriod(float value);
	/*!
	 * @brief 1カーブ辺りの大きさを設定
	 */
	void SetAmplitude(float value);

	/*!
	 * @brief 縦の移動値を取得
	 */
	float GetOffset() const { return offset_; }
	/*!
	 * @brief 横の移動値を設定
	 */
	float GetPeriod() const { return period_; }
	/*!
	 * @brief 1カーブ辺りの大きさを設定
	 */
	float GetAmplitude() const { return amplitude_; }

private :
	D3DXHANDLE offset_handle_;
	D3DXHANDLE period_handle_;
	D3DXHANDLE amplitude_handle_;
	
	float offset_;
	float period_;
	float amplitude_;
};
