#pragma once

#include "Shader.h"
#include <string>

/*!
 * @brief シェーダ
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
	virtual void SetTexture(Texture* texture);
	void SetDispersion(float dispersion);
	float GetDispersion() const;

private :
	static const int WEIGHT_MUN = 8;

	void UpdateWeight_(float dispersion);

	D3DXHANDLE weight_;
	D3DXHANDLE src_map_;
	float dispersion_;
	float weight_table_[WEIGHT_MUN];
};
