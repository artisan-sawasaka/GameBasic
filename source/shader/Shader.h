#pragma once

#include "main/DeviceLostListener.h"
#include "render/Texture.h"
#include "render/Color.h"
#include <d3dx9.h>
#include <string>

/*!
 * @brief シェーダ
 */
class Shader : public DeviceLostListener
{
public :
	Shader();
	~Shader();

	virtual bool LoadFile(const char* path);
	virtual void Release();
	virtual void SetTexture(Texture* texture);
	virtual void SetColor(const Color& color, float rate = 1.0f);
	const Color& GetColor() const { return color_; }
	void Begin(int index = 0);
	void End();
	void OnLostDevice();
	void OnResetDevice();

protected :
	void InitializeHandle_();
	bool Create_(const char* src);

	LPD3DXEFFECT effect_;
	D3DXHANDLE techniqe_;
	D3DXHANDLE src_map_;
	D3DXHANDLE diffuse_handle_;
	Color color_;
};
