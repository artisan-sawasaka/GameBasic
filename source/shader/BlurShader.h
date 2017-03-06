#pragma once

#include "Shader.h"
#include <string>

/*!
 * @brief ブラーシェーダ
 */
class BlurShader : public Shader
{
public :
	enum PassType {
		BlurX,
		BlurY,
		BlurXY,
	};

	BlurShader();
	~BlurShader();

	virtual bool LoadFile(const char* path) { return false; }
	void SetDispersion(float dispersion);
	float GetDispersion() const;

private :
	static const int WEIGHT_MUN = 8;

	void UpdateWeight_(float dispersion);

	D3DXHANDLE weight_;
	float dispersion_;
	float weight_table_[WEIGHT_MUN];
};
