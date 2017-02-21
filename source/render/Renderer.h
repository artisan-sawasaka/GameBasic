/*!
 * @file
 *
 * @brief レンダリングクラスを提供します。
 *
 * @copyright
 */
#pragma once

#include <Windows.h>
#include <gdiplus.h>
#include <memory>
#include "master/MasterData.hpp"
#include "Texture.h"
#include "Color.h"

class AppBase;

/*!
 * @brief レンダリングクラス
 */
class Renderer
{
public :
	enum Anchor
	{
		LEFT_TOP,
		TOP,
		RIGHT_TOP,
		LEFT,
		CENTER,
		RIGHT,
		LEFT_BOTTOM,
		BOTTOM,
		RIGHT_BOTTOM,
	};

	~Renderer();

	/*!
	 * @brief 初期化
	 */
	void Initialize(AppBase* app);

	/*!
	 * @brief 画面クリア
	 */
	void ClearScreen(const Color& color);

	/*!
	 * @brief 塗りつぶし描画
	 */
	void FillRect(int x, int y, int w, int h, const Color& color);

	/*!
	 * @brief 画像描画
	 */
	void DrawImage(Texture* texture, Anchor anchor, int x, int y);

	/*!
	 * @brief 画像描画
	 */
	void DrawImage(Texture* texture, Anchor anchor, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, const Color& color = Color::White, float rotate = 0.0f);

	/*!
	 * @brief 文字列描画
	 */
	void DrawString(const char* s, Anchor anchor, int x, int y, int size = 20, const Color& color = Color::White);

	/*!
	 * @brief 文字列描画
	 */
	void DrawString(const wchar_t* s, Anchor anchor, int x, int y, int size = 20, const Color& color = Color::White);

	/*!
	 * @brief フォーマット指定文字列描画
	 */
	void DrawStringFormat(Anchor anchor, int x, int y, int size, const Color& color, const char* s, ...);

	/*!
	 * @brief フォーマット指定文字列描画
	 */
	void DrawStringFormat(Anchor anchor, int x, int y, int size, const Color& color, const wchar_t* s, ...);

	/*!
	 * @brief インスタンス取得
	 */
	static Renderer* GetInstance() {
		static Renderer v;
		return &v;
	}

private :
	Renderer();

	AppBase* app_;
};
