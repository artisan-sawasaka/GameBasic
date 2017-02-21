#include "Renderer.h"
#include "utility/Utility.hpp"
#include "main/WinMain.h"
#include <string.h>
#include <vector>
#include <string>

static const float RotateBase = 6.28318530718f;


D3DCMPFUNC ChangeFunc(Renderer::FUNC func)
{
	if (func == Renderer::NEVER) return D3DCMP_NEVER;
	if (func == Renderer::ALWAYS) return D3DCMP_ALWAYS;
	if (func == Renderer::LESS) return D3DCMP_LESS;
	if (func == Renderer::EQUAL) return D3DCMP_EQUAL;
	if (func == Renderer::LESSEQUAL) return D3DCMP_LESSEQUAL;
	if (func == Renderer::GREATER) return D3DCMP_GREATER;
	if (func == Renderer::NOTEQUAL) return D3DCMP_NOTEQUAL;
	if (func == Renderer::GREATEREQUAL) return D3DCMP_GREATEREQUAL;
	return D3DCMP_FORCE_DWORD;
}

uint32_t ChangeStencilCaps(Renderer::STENCIL_CAPS caps)
{
	if (caps == Renderer::STENCIL_CAPS_KEEP) return D3DSTENCILCAPS_KEEP;
	if (caps == Renderer::STENCIL_CAPS_ZERO) return D3DSTENCILCAPS_ZERO;
	if (caps == Renderer::STENCIL_CAPS_REPLACE) return D3DSTENCILCAPS_REPLACE;
	if (caps == Renderer::STENCIL_CAPS_INVERT) return D3DSTENCILCAPS_INVERT;

	return 0;
}

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

/*!
 * @brief 初期化
 */
void Renderer::Initialize(AppBase* app)
{
	app_ = app;
}

void Renderer::SetTextureFilter(bool filter)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (filter) {
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);		// ミップマップ
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);		// 拡大
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);		// 縮小
	} else {
		device->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_NONE);		// ミップマップ
		device->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_NONE);		// 拡大
		device->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_NONE);		// 縮小
	}
}

void Renderer::SetAlphaTest(bool enable, FUNC func, uint8_t ref)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (enable) {
		D3DCMPFUNC f = ChangeFunc(func);
		if (f == D3DCMP_FORCE_DWORD) return ;

		device->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		device->SetRenderState(D3DRS_ALPHAFUNC, f);
		device->SetRenderState(D3DRS_ALPHAREF, ref);
	} else {
		device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}

void Renderer::SetBlend(BLEND blend)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (blend == BLEND_DISABLE) {
		// 無効
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
	} else if (blend == BLEND_ALPHA) {
		// アルファブレンド
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	} else if (blend == BLEND_ADD) {
		// 加算
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
	} else if (blend == BLEND_SUB) {
		// 減算
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_INVDESTCOLOR);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
	} else if (blend == BLEND_MUL) {
		// 乗算
		device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		device->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ZERO);
		device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCCOLOR);
	}
}

void Renderer::SetTextureUse(TEXTURE_USE tex)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (tex == TEXTURE_USE_DISABLE) {
		// 無効
	} else if (tex == TEXTURE_USE_ENABLE) {
		// 有効
	} else if (tex == TEXTURE_USE_COLOR) {
		// カラーのみ有効
	} else if (tex == TEXTURE_USE_ALPHA) {
		// アルファのみ有効
	}
}

void Renderer::SetStencilState(STENCIL_STATAE stencil)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (stencil == STENCIL_DISABLE) {
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	} else if (stencil == STENCIL_MASK) {
		// ステンシル設定
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_STENCILREF,    0x01);
		device->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_REPLACE);
		device->SetRenderState(D3DRS_STENCILZFAIL,  D3DSTENCILOP_REPLACE);
		device->SetRenderState(D3DRS_STENCILMASK,   0xff);
	}else if (stencil == STENCIL_UNMASK) {
		// ステンシル設定
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_ALWAYS);
		device->SetRenderState(D3DRS_STENCILREF,    0x00);
		device->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_REPLACE);
		device->SetRenderState(D3DRS_STENCILZFAIL,  D3DSTENCILOP_REPLACE);
		device->SetRenderState(D3DRS_STENCILMASK,   0xff);
	}else if (stencil == STENCIL_DRAW) {
		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC,   D3DCMP_EQUAL);
		device->SetRenderState(D3DRS_STENCILREF,    0x00);
		device->SetRenderState(D3DRS_STENCILPASS,   D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_STENCILZFAIL,  D3DSTENCILOP_KEEP);
		device->SetRenderState(D3DRS_STENCILMASK,   0xff);
	}
}

void Renderer::SetStencil(bool enable, FUNC func, uint8_t ref, STENCIL_CAPS sfail, STENCIL_CAPS zfail, STENCIL_CAPS zpass)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (enable) {
		D3DCMPFUNC f = ChangeFunc(func);
		uint32_t s1 = ChangeStencilCaps(sfail);
		uint32_t z1 = ChangeStencilCaps(zfail);
		uint32_t z2 = ChangeStencilCaps(zpass);
		if (f == D3DCMP_FORCE_DWORD || s1 == 0 || z1 == 0 || z2 == 0) return ;

		device->SetRenderState(D3DRS_STENCILENABLE, TRUE);
		device->SetRenderState(D3DRS_STENCILFUNC,   f);
		device->SetRenderState(D3DRS_STENCILMASK,   0xff);
		device->SetRenderState(D3DRS_STENCILFAIL,   s1);
		device->SetRenderState(D3DRS_STENCILPASS,   z1);
		device->SetRenderState(D3DRS_STENCILZFAIL,  z2);
	} else {
		device->SetRenderState(D3DRS_STENCILENABLE, FALSE);
	}
}

void Renderer::SetZEnable(bool enable, FUNC func)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	if (enable) {
		D3DCMPFUNC f = ChangeFunc(func);
		if (f == D3DCMP_FORCE_DWORD) return ;

		device->SetRenderState(D3DRS_ZENABLE, D3DZB_USEW);
		device->SetRenderState(D3DRS_ZFUNC, f);
	} else {
		device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
	}
}

void Renderer::SetZWriteEnable(bool enable)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	device->SetRenderState(D3DRS_ZWRITEENABLE, enable ? TRUE : FALSE);
}

/*!
 * @brief 画面クリア
 */
void Renderer::ClearScreen(const Color& color)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	device->Clear(0, NULL, D3DCLEAR_STENCIL | D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, color.GetARGB(), 1.0f, 0);
}

/*!
 * @brief 塗りつぶし描画
 */
void Renderer::FillRect(int x, int y, int w, int h, const Color& color)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;

	const float left = static_cast<float>(x);
	const float top = static_cast<float>(y);
	const float right = static_cast<float>(w) + left;
	const float bottom = static_cast<float>(h) + top;
	const uint32_t col = color.GetARGB();

	struct VERTEX2D {
		float x, y, z, rhw;
		uint32_t color;
	} v[] = {
		left,  top,    0.0f, 1.0f, col,
		right, top,    0.0f, 1.0f, col,
		left,  bottom, 0.0f, 1.0f, col,
		right, bottom, 0.0f, 1.0f, col,
	};

	HRESULT hr;
	hr = device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	hr = device->SetTexture(0, NULL);
	hr = device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX2D));
}

/*!
 * @brief 画像描画
 */
void Renderer::DrawImage(Texture* texture, Anchor anchor, int x, int y)
{
	if (texture == nullptr) return ;
	DrawImage(texture, anchor, x, y, texture->GetWidth(), texture->GetHeight(), 0, 0, texture->GetWidth(), texture->GetHeight());
}

/*!
 * @brief 画像描画
 */
void Renderer::DrawImage(Texture* texture, Anchor anchor, int dx, int dy, int dw, int dh,
	int sx, int sy, int sw, int sh, const Color& color, float rotate)
{
	auto device = app_->GetDevice().GetDevice();
	if (device == nullptr) return ;
	if (texture == nullptr) return ;
	if (color.GetA() == 0) return ;

	const float left = static_cast<float>(dx);
	const float top = static_cast<float>(dy);
	const float right = static_cast<float>(dw) + left;
	const float bottom = static_cast<float>(dh) + top;
	const float tl = static_cast<float>(sx + 0.5f) / texture->GetWidth();
	const float tt = static_cast<float>(sy + 0.5f) / texture->GetHeight();
	const float tr = static_cast<float>(sw) / texture->GetWidth() + tl;
	const float tb = static_cast<float>(sh) / texture->GetHeight() + tt;
	const uint32_t col = color.GetARGB();

	struct VERTEX2D {
		float x, y, z, rhw;
		uint32_t color;
		float u, v;
	} v[] = {
		left,  top,    0.0f, 1.0f, col, tl, tt,
		right, top,    0.0f, 1.0f, col, tr, tt,
		left,  bottom, 0.0f, 1.0f, col, tl, tb,
		right, bottom, 0.0f, 1.0f, col, tr, tb,
	};

	float mx = static_cast<float>((dw >> 1) * (anchor % 3));
	float my = static_cast<float>((dh >> 1) * (anchor / 3));
		
	// 回転行列を書き込む
	D3DXMATRIX mat, matt;
	D3DXMatrixTranslation(&mat, -mx, -my, 0);
	if (rotate != 0) {
		mat *= *D3DXMatrixRotationZ(&matt, rotate * RotateBase);
	}

	// 適応
	for (int i = 0; i < 4; ++i) {
		D3DXVECTOR3 din, dout;
		din.x = v[i].x;
		din.y = v[i].y;
		din.z = v[i].z;
		D3DXVec3TransformCoord(&dout, &din, &mat);
		v[i].x = dout.x;
		v[i].y = dout.y;
		v[i].z = dout.z;
	}

	HRESULT hr;
	hr = device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1);
	hr = device->SetTexture(0, texture->GetTexture());
	hr = device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(VERTEX2D));
}

/*!
 * @brief 文字列描画
 */
void Renderer::DrawString(const char* s, Anchor anchor, int x, int y, int size, const Color& color)
{
	if (color.GetA() == 0) return ;

	this->DrawString(Utility::SJIStoUTF16(s).c_str(), anchor, x, y, size, color);
}

/*!
 * @brief 文字列描画
 */
void Renderer::DrawString(const wchar_t* s, Anchor anchor, int x, int y, int size, const Color& color)
{
	if (color.GetA() == 0) return ;

}

/*!
 * @brief 文字描画
 */
void Renderer::DrawStringFormat(Anchor anchor, int x, int y, int size, const Color& color, const char* s, ...)
{
	if (color.GetA() == 0) return ;

	// テキストフォーマットを変換
	char text[10240];
	va_list arg;
	va_start(arg, s);
	vsprintf(text, s, arg);
	va_end(arg);

	this->DrawString(Utility::SJIStoUTF16(text).c_str(), anchor, x, y, size, color);
}

/*!
 * @brief 文字描画
 */
void Renderer::DrawStringFormat(Anchor anchor, int x, int y, int size, const Color& color, const wchar_t* s, ...)
{
	if (color.GetA() == 0) return ;

	// テキストフォーマットを変換
	wchar_t text[10240];
	va_list arg;
	va_start(arg, s);
	vswprintf(text, s, arg);
	va_end(arg);

	this->DrawString(text, anchor, x, y, size, color);
}
