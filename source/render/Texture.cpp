#include "Texture.h"
#include "utility/DeviceManager.h"
#include <memory>

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

/*!
 * @brief コンストラクタ
 */
Texture::Texture()
	: texture_(nullptr)
{
	std::memset(&lock_rect_, 0, sizeof(lock_rect_));
	std::memset(&desc_, 0, sizeof(desc_));
	std::memset(&info_, 0, sizeof(info_));
}

/*!
 * @brief デストラクタ
 */
Texture::~Texture()
{
	SAFE_RELEASE(texture_);
}

/*!
 * @brief テクスチャーをデバイスに適応します。
 */
void Texture::Apply(int index)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr) return ;

	device->SetTexture(index, texture_);
}

/*!
 * @brief ファイルからテクスチャーを作成します。
 */
bool Texture::CreateFromFile(const char* path, MIPMAP mipmap)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr) return false;
	if (path == nullptr) return false;

	SAFE_RELEASE(texture_);

	HRESULT hr;

	// 画像の情報を取得
	hr = D3DXGetImageInfoFromFileA(path, &info_);
	if (FAILED(hr)) {
		return false;
	}

	uint32_t mm = 0;
	if (mipmap == MIPMAP_DEFAULT) {
		mm = info_.MipLevels;
	} else if (mipmap == MIPMAP_ENABLE) {
		mm = D3DX_DEFAULT;
	} else if (mipmap == MIPMAP_DISABLE) {
		mm = 1;
	}

	// テクスチャーの作成
	hr = D3DXCreateTextureFromFileExA(device, path, info_.Width, info_.Height,
									 mm, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_POINT,
									 D3DX_DEFAULT, 0, nullptr, nullptr, &texture_);
	if (FAILED(hr)) {
		return false;
	}

	// テクスチャー情報の取得
	hr = texture_->GetLevelDesc(0, &desc_);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

/*!
 * @brief メモリからテクスチャーを作成します。
 */
bool Texture::CreateFromMemory(const char* buffer, uint32_t size, MIPMAP mipmap)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr) return false;
	if (buffer == nullptr) return false;

	SAFE_RELEASE(texture_);

	// 画像の情報を取得
	HRESULT hr = D3DXGetImageInfoFromFileInMemory(buffer, size, &info_);
	if (FAILED(hr)) {
		return false;
	}

	uint32_t mm = 0;
	if (mipmap == MIPMAP_DEFAULT) {
		mm = info_.MipLevels;
	} else if (mipmap == MIPMAP_ENABLE) {
		mm = D3DX_DEFAULT;
	} else if (mipmap == MIPMAP_DISABLE) {
		mm = 1;
	}

	// テクスチャーの作成
	hr = D3DXCreateTextureFromFileInMemoryEx(device, buffer, size, info_.Width, info_.Height,
											 mm, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_FILTER_POINT,
											 D3DX_DEFAULT, 0, nullptr, nullptr, &texture_);
	if (FAILED(hr)) {
		return false;
	}

	// テクスチャー情報の取得
	hr = texture_->GetLevelDesc(0, &desc_);
	if (FAILED(hr)) {
		return false;
	}

	return true;
}

/*!
 * @brief レンダリングターゲットを作成します。
 */
bool Texture::CreateRenderTarget(TEXTURE_FORMAT format, uint32_t width, uint32_t height)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr) return false;

	SAFE_RELEASE(texture_);

	D3DFORMAT fmt;
	switch (format) {
		case TEXTURE_FORMAT_RGB565 : fmt = D3DFMT_R5G6B5; break;
		case TEXTURE_FORMAT_XRGB8888 : fmt = D3DFMT_X8R8G8B8; break;
		case TEXTURE_FORMAT_ARGB8888 : fmt = D3DFMT_A8R8G8B8; break;
		default : return false;
	}
	HRESULT hr;

	hr = device->CreateTexture(width, height, 1, D3DUSAGE_RENDERTARGET, fmt, D3DPOOL_DEFAULT, &texture_, nullptr);
	if (FAILED(hr)) return false;

	// バックバッファのサーフェイス情報を取得
	LPDIRECT3DSURFACE9 surface;
	D3DSURFACE_DESC desc;
	hr = device->GetDepthStencilSurface(&surface);
	if (FAILED(hr)) return false;

	hr = surface->GetDesc(&desc);
	if (FAILED(hr)) {
		SAFE_RELEASE(surface);
		return false;
	}
	SAFE_RELEASE(surface);

	
	hr = texture_->GetSurfaceLevel(0, &surface);
	if (FAILED(hr)) {
		return false;
	}

	// レンダリングターゲットを生成
	hr = device->CreateRenderTarget(width, height, fmt, desc.MultiSampleType, 0, FALSE, &surface, nullptr);
	SAFE_RELEASE(surface);
	if (FAILED(hr))	return false;

	// テクスチャー情報の取得
	hr = texture_->GetLevelDesc(0, &desc_);
	if (FAILED(hr)) return false;

	return true;
}
