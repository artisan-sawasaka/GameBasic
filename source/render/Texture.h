/*!
 * @file
 *
 * @brief テクスチャークラスを提供します。
 *
 * @copyright
 */
#pragma once

#include <d3dx9.h>
#include <cstdint>
#include "TextureFormat.h"
#include "main/DeviceLostListener.h"

/*!
 * @brief テクスチャークラス
 */
class Texture : public DeviceLostListener
{
public :
	Texture();
	~Texture();

	void Apply(int index = 0);
	void ApplyRenderTarget();
	void ResetRenderTarget();
	void Release();
	bool CreateFromFile(const char* path, MIPMAP mipmap = MIPMAP_DEFAULT);
	bool CreateFromMemory(const char* buffer, uint32_t size, MIPMAP mipmap = MIPMAP_DEFAULT);
	bool CreateRenderTarget(TEXTURE_FORMAT format, uint32_t width, uint32_t height);
	uint32_t GetWidth() const { return desc_.Width; }
	uint32_t GetHeight() const { return desc_.Height; }
	LPDIRECT3DTEXTURE9 GetTexture() const  { return texture_; }
	void OnLostDevice();
	void OnResetDevice();

private :
	Texture& operator=(const Texture&);
	LPDIRECT3DTEXTURE9 texture_;
	D3DSURFACE_DESC desc_;
	D3DXIMAGE_INFO info_;
	LPDIRECT3DSURFACE9 surface_;
	bool is_render_target_;

	TEXTURE_FORMAT render_target_format_;
	uint32_t render_target_width_;
	uint32_t render_target_height_;
};
