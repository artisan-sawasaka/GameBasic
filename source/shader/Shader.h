#pragma once

#include "main/DeviceLostListener.h"
#include "render/Texture.h"
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

	void Begin(int index = 0);
	void End();
	void OnLostDevice();
	void OnResetDevice();

protected :
	LPD3DXEFFECT effect_;
	D3DXHANDLE techniqe_;
	D3DXHANDLE src_map_;
	Texture* texture_;
};
