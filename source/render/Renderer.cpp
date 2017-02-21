#include "Renderer.h"
#include "utility/Utility.hpp"
#include "main/WinMain.h"
#include <string.h>
#include <vector>
#include <string>

static const float RotateBase = 6.28318530718f;

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
	mat.m[3][0] += mx;
	mat.m[3][1] += my;

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
