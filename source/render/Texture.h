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

/*!
 * @brief テクスチャークラス
 */
class Texture
{
public :
	Texture();
	~Texture();

	bool CreateFromFile(const char* path, MIPMAP mipmap = MIPMAP_DEFAULT);
	bool CreateFromMemory(const char* buffer, uint32_t size, MIPMAP mipmap = MIPMAP_DEFAULT);
	bool CreateRenderTarget(TEXTURE_FORMAT format, uint32_t width, uint32_t height);
	uint32_t GetWidth() const { return desc_.Width; }
	uint32_t GetHeight() const  { return desc_.Height; }
	LPDIRECT3DTEXTURE9 GetTexture() { return texture_; }

private :
	LPDIRECT3DTEXTURE9 texture_;
	D3DLOCKED_RECT lock_rect_;
	D3DSURFACE_DESC desc_;
	D3DXIMAGE_INFO info_;
};
