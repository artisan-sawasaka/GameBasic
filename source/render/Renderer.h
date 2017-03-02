/*!
 * @file
 *
 * @brief レンダリングクラスを提供します。
 *
 * @copyright
 */
#pragma once

#include <Windows.h>
#include <memory>
#include <d3d9.h>
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

	enum BLEND
	{
		BLEND_DISABLE,			//!< 無効
		BLEND_ALPHA,			//!< アルファブレンド
		BLEND_ADD,				//!< 加算
		BLEND_SUB,				//!< 減算
		BLEND_MUL,				//!< 乗算
	};

	enum TEXTURE_USE
	{
		TEXTURE_USE_DISABLE,	//!< 無効
		TEXTURE_USE_ENABLE,		//!< 有効
		TEXTURE_USE_COLOR,		//!< カラーのみ有効
		TEXTURE_USE_ALPHA,		//!< アルファのみ有効
	};

	enum STENCIL_STATAE
	{
		STENCIL_DISABLE,		//!< 無効
		//!< 以下ステンシルは有効設定です。
		STENCIL_MASK,			//!< マスクの作成
		STENCIL_UNMASK,			//!< マスクの作成
		STENCIL_DRAW,			//!< 適用済のマスクに従い描画範囲クリッピングします
	};

	enum STENCIL_CAPS
	{
		STENCIL_CAPS_KEEP,		//!< ステンシルバッファの項目を更新しない
		STENCIL_CAPS_ZERO,		//!< ステンシルバッファの項目を0に設定する
		STENCIL_CAPS_REPLACE,	//!< ステンシルバッファの項目を基準値で置き換える
		STENCIL_CAPS_INVERT,	//!< ステンシルバッファの項目を反転する
	};

	enum FUNC
	{
		NEVER,					//!< テストは常に失敗する。
		ALWAYS,					//!< テストは常に成功する。
		EQUAL,					//!< 新しいピクセル値が、現在のピクセル値と等しいときに応じる。
		NOTEQUAL,				//!< 新しいピクセル値が、現在のピクセル値と等しくないときに応じる。
		LESS,					//!< 新しいピクセル値が、現在のピクセル値より小さいときに応じる。 
		LESSEQUAL,				//!< 新しいピクセル値が、現在のピクセル値以下のときに応じる。
		GREATER,				//!< 新しいピクセル値が、現在のピクセル値より大きいときに応じる。
		GREATEREQUAL,			//!< 新しいピクセル値が、現在のピクセル値以上のときに応じる。
	};

	struct Vertex2D {
		float x, y, z, rhw;
		uint32_t color;
		float u, v;
	};

	~Renderer();

	/*!
	 * @brief 初期化
	 */
	void Initialize(AppBase* app);

	/*!
	 * @brief 終了化
	 */
	void Finalize();

	/*!
	 * @brief テクスチャーフィルタの有無を設定します。
	 *
	 * @param filter テクスチャーフィルタの有無を指定します。
	 *
	 * @return なし
	 */
	void SetTextureFilter(bool filter);

	/*!
	 * @brief アルファテストの有無を設定します。
	 *
	 * @param enable アルファテストの有無を指定します。
	 * @param func 判定関数を指定します。
	 * @param ref テストの値を指定します。
	 *
	 * @return なし
	 */
	void SetAlphaTest(bool enable, FUNC func = NOTEQUAL, uint8_t ref = 0);

	/*!
	 * @brief ブレンドモードを変更します。
	 *
	 * @param blend ブレンドモードを指定します。
	 *
	 * @return なし
	 */
	void SetBlend(BLEND blend);

	/*!
	 * @brief テクスチャーの使用状態を変更します。
	 *
	 * @param tex テクスチャーの使用状態を指定します。
	 *
	 * @return なし
	 */
	void SetTextureUse(TEXTURE_USE tex);

	/*!
	 * @brief ステンシルステータスを設定します
	 *
	 * @param stencil ステンシルステータス内容を指定します
	 *
	 * @return なし
	 */
	void SetStencilState(STENCIL_STATAE stencil);

	/*!
	 * @brief ステンシルステータスを設定します
	 *
	 * @param enable ステンシルの有無を指定します。
	 * @param func 判定関数を指定します。
	 * @param ref テストの値を指定します。
	 * @param sfail ステンシルテスト失敗時のオペレーションを指定します。
	 * @param zfail ステンシルテスト成功かつデプステスト失敗時のオペレーションを指定します。
	 * @param zpass ステンシルテスト成功かつデプステスト成功時のオペレーションを指定します。
	 *
	 * @return なし
	 */
	void SetStencil(bool enable, FUNC func = NOTEQUAL, uint8_t ref = 0, STENCIL_CAPS sfail = STENCIL_CAPS_KEEP, STENCIL_CAPS zfail = STENCIL_CAPS_KEEP, STENCIL_CAPS zpass = STENCIL_CAPS_KEEP);

	/*!
	 * @brief Zバッファの有無を設定します。
	 *
	 * @param enable ステンシルステータス内容を指定します
	 *
	 * @return なし
	 */
	void SetZEnable(bool enable, FUNC func = LESSEQUAL);

	/*!
	 * @brief Zバッファの書き込みの有無を設定します。
	 *
	 * @param enable Zバッファの書き込みの有無を指定します
	 *
	 * @return なし
	 */
	void SetZWriteEnable(bool enable);

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
	 * @brief ストッククリア
	 */
	void ClearStock();

	/*!
	 * @brief 画像ストック
	 */
	void ImageStock(Texture* texture, Anchor anchor, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, const Color& color = Color::White, float rotate = 0.0f);

	/*!
	 * @brief ストック描画
	 */
	void DrawStock();

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
	LPD3DXFONT GetFont_(int size);
	void CreateVertex2D_(Vertex2D* v, Texture* texture, Anchor anchor, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh, const Color& color, float rotate);
	LPDIRECT3DDEVICE9 GetDevice_();

	AppBase* app_;
	std::map<int, LPD3DXFONT> fonts_;
	std::vector<Vertex2D> stocks_;
	size_t stock_count_;
};
