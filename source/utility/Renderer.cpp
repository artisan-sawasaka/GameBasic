#pragma once

#include "Renderer.h"
#include "Utility.hpp"
#include <string.h>
#include <vector>
#include <string>

Renderer::Renderer()
{
	// GDI+初期化
	Gdiplus::GdiplusStartup(&token_, &gpsi_, NULL);
}

Renderer::~Renderer()
{
	brush_.reset();
	fonts_.clear();
	graphics_.reset();
	Gdiplus::GdiplusShutdown(token_);
}

/*!
 * @brief 初期化
 */
void Renderer::Initialize(HDC hdc)
{
	graphics_.reset(new Gdiplus::Graphics(hdc));
	brush_.reset(new Gdiplus::SolidBrush(Gdiplus::Color::White));
	memset(cm_.m, 0, sizeof(cm_.m));
	cm_.m[4][4] = 1.0f;
	string_format_.SetFormatFlags(Gdiplus::StringFormatFlagsNoClip);
}

/*!
 * @brief 画面クリア
 */
void Renderer::ClearScreen(const Gdiplus::Color& color)
{
	graphics_->Clear(color);
}

/*!
 * @brief 塗りつぶし描画
 */
void Renderer::FillRect(int x, int y, int w, int h, const Gdiplus::Color& color)
{
	brush_->SetColor(color);
	graphics_->FillRectangle(brush_.get(), x, y, w, h);
}

/*!
 * @brief 画像描画
 */
void Renderer::DrawImage(Gdiplus::Bitmap* image, Anchor anchor, int x, int y)
{
	// アンカーを基準にして表示座標を変える
	if (anchor != LEFT_TOP) {
		x -= (image->GetWidth() >> 1) * (anchor % 3);
		y -= (image->GetHeight() >> 1) * (anchor / 3);
	}

	// 描画
	graphics_->DrawImage(image, x, y, image->GetWidth(), image->GetHeight());
}

/*!
 * @brief 画像描画
 */
void Renderer::DrawImage(Gdiplus::Bitmap* image, Anchor anchor, int dx, int dy, int dw, int dh,
	int sx, int sy, int sw, int sh, const Gdiplus::Color& color)
{
	if (color.GetA() == 0) return ;

	// 色設定
	cm_.m[0][0] = color.GetR() / 255.0f;
	cm_.m[1][1] = color.GetG() / 255.0f;
	cm_.m[2][2] = color.GetB() / 255.0f;
	cm_.m[3][3] = color.GetA() / 255.0f;
	ia_.SetColorMatrix(&cm_);


	// アンカーを基準にして表示座標を変える
	if (anchor != LEFT_TOP) {
		dx -= (dw >> 1) * (anchor % 3);
		dy -= (dh >> 1) * (anchor / 3);
	}

	// 描画
	Gdiplus::Rect dst(dx, dy, dw, dh);
	graphics_->DrawImage(image, dst, sx, sy, sw, sh, Gdiplus::UnitPixel, & ia_);
}

/*!
 * @brief 文字列描画
 */
void Renderer::DrawString(const char* s, Anchor anchor, int x, int y, int size, const Gdiplus::Color& color)
{
	if (color.GetA() == 0) return ;

	this->DrawString(Utility::SJIStoUTF16(s).c_str(), anchor, x, y, size, color);
}

/*!
 * @brief 文字列描画
 */
void Renderer::DrawString(const wchar_t* s, Anchor anchor, int x, int y, int size, const Gdiplus::Color& color)
{
	if (color.GetA() == 0) return ;

	Gdiplus::Font* font = GetFont(size);
	brush_->SetColor(color);

	// アンカーを基準にして表示座標を変える
	if (anchor != LEFT_TOP) {
		Gdiplus::RectF range;
		graphics_->MeasureString(s, wcslen(s), font, Gdiplus::PointF(), &string_format_, &range);
		x -= static_cast<int>(range.Width)  / 2 * (anchor % 3);
		y -= static_cast<int>(range.Height) / 2 * (anchor / 3);
	}

	graphics_->DrawString(s, wcslen(s), font, Gdiplus::PointF(static_cast<float>(x), static_cast<float>(y)), &string_format_, brush_.get());
}

/*!
 * @brief 文字描画
 */
void Renderer::DrawStringFormat(Anchor anchor, int x, int y, int size, const Gdiplus::Color& color, const char* s, ...)
{
	if (color.GetA() == 0) return ;

	// テキストフォーマットを変換
	char text[1024];
	va_list arg;
	va_start(arg, s);
	vsprintf(text, s, arg);
	va_end(arg);

	this->DrawString(Utility::SJIStoUTF16(text).c_str(), anchor, x, y, size, color);
}

/*!
 * @brief 文字描画
 */
void Renderer::DrawStringFormat(Anchor anchor, int x, int y, int size, const Gdiplus::Color& color, const wchar_t* s, ...)
{
	if (color.GetA() == 0) return ;

	// テキストフォーマットを変換
	wchar_t text[1024];
	va_list arg;
	va_start(arg, s);
	vswprintf(text, s, arg);
	va_end(arg);

	this->DrawString(text, anchor, x, y, size, color);
}

/*!
 * @brief フォントの取得
 */
Gdiplus::Font* Renderer::GetFont(int size)
{
	auto it = fonts_.find(size);
	if (it != fonts_.end()) {
		return fonts_[size].get();
	}
	auto p = new Gdiplus::Font(L"ＭＳ ゴシック" , static_cast<Gdiplus::REAL>(size));
	fonts_.insert(std::pair<int, std::shared_ptr<Gdiplus::Font>>(size, p));
	return p;
}
