/*!
 * @file
 * 
 * @brief カラークラスを提供します
 * 
 * @copyright
 */
#pragma once

#include <cstdint>

/*!
 * @brief RGB888形式のピクセルフォーマットを表すクラスです。
 */
class Color{
public :
	static Color Black;
	static Color White;
	static Color Red;

public :
	/*!
	 * @brief コンストラクタ
	 *
	 *        0 で初期化されます。
	 *
	 * @param なし
	 */
	Color(){ Clear(); }

	/*!
	 * @brief コンストラクタ
	 *
	 * @param argb ARGB8888形式の値を指定します。
	 */
	Color(uint32_t argb){ SetARGB(argb); }

	/*!
	 * @brief コンストラクタ
	 * 
	 * @param r 赤の色を @b 0～255 で指定します。
	 * @param g 緑の色を @b 0～255 で指定します。
	 * @param b 青の色を @b 0～255 で指定します。
	 */
	Color(uint8_t r, uint8_t g, uint8_t b){ SetARGB(255, r, g, b); }

	/*!
	 * @brief コンストラクタ
	 * 
	 * @param a アルファを @b 0～255 で指定します。
	 * @param r 赤の色を @b 0～255 で指定します。
	 * @param g 緑の色を @b 0～255 で指定します。
	 * @param b 青の色を @b 0～255 で指定します。
	 */
	Color(uint8_t a, uint8_t r, uint8_t g, uint8_t b){ SetARGB(a, r, g, b); }

	/*!
	 * @brief 赤を取得します。
	 *
	 * @param なし
	 *
	 * @return 赤の色を返します。
	 */
	uint8_t GetR() const{ return r_; }

	/*!
	 * @brief 赤をセットします。
	 *
	 * @param r 赤の色を @b 0～255 で指定します。
	 *
	 * @return なし
	 */
	void SetR(uint8_t r){ r_ = r; }

	/*!
	 * @brief 緑を取得します。
	 *
	 * @param なし
	 *
	 * @return 緑の色を返します。
	 */
	uint8_t GetG() const{ return g_; }

	/*!
	 * @brief 緑をセットします。
	 *
	 * @param g 緑の色を @b 0～255 で指定します。
	 *
	 * @return なし
	 */
	void SetG(uint8_t g){ g_ = g; }

	/*!
	 * @brief 青を取得します。
	 *
	 * @param なし
	 *
	 * @return 青の色を返します。
	 */
	uint8_t GetB() const{ return b_; }

	/*!
	 * @brief 青をセットします。
	 *
	 * @param b 青の色を @b 0～255 で指定します。
	 *
	 * @return なし
	 */
	void SetB(uint8_t b){ b_ = b; }

	/*!
	 * @brief アルファを取得します。
	 *
	 * @param なし
	 *
	 * @return アルファの値を返します。
	 */
	uint8_t GetA() const{ return a_; }

	/*!
	 * @brief アルファをセットします。
	 *
	 * @param a アルファの値を @b 0～255 で指定します。
	 *
	 * @return なし
	 */
	void SetA(uint8_t a) { a_ = a; }

	/*!
	 * @brief ピクセルをセットします。
	 * 
	 * @param r 赤の色を @b 0～255 で指定します。
	 * @param g 緑の色を @b 0～255 で指定します。
	 * @param b 青の色を @b 0～255 で指定します。
	 *
	 * @return なし
	 */
	void SetRGB(uint8_t r, uint8_t g, uint8_t b){ r_ = r; g_ = g; b_ = b; }

	/*!
	 * @brief ピクセルをセットします。
	 * 
	 * @param rgb RGB888形式の値を指定します。
	 *
	 * @return なし
	 */
	void SetRGB(uint32_t rgb){ SetRGB((uint8_t)(rgb >> 0), (uint8_t)(rgb >> 8), (uint8_t)(rgb >> 16)); }

	/*!
	 * @brief ピクセルを取得します。
	 * 
	 * @param なし
	 *
	 * @return ピクセルの値をRGB888形式で返します。
	 */
	uint32_t GetRGB() const{ return (b_ << 16) | (g_ << 8) | (r_ << 0); }

	/*!
	 * @brief ピクセルをセットします。
	 * 
	 * @param a アルファを @b 0～255 で指定します。
	 * @param r 赤の色を @b 0～255 で指定します。
	 * @param g 緑の色を @b 0～255 で指定します。
	 * @param b 青の色を @b 0～255 で指定します。
	 *
	 * @return なし
	 */
	void SetARGB(uint8_t a, uint8_t r, uint8_t g, uint8_t b){ a_ = a; r_ = r; g_ = g; b_ = b; }

	/*!
	 * @brief ピクセルをセットします。
	 * 
	 * @param argb ARGB8888形式の値を指定します。
	 *
	 * @return なし
	 */
	void SetARGB(uint32_t argb){ SetARGB((uint8_t)(argb >> 24), (uint8_t)(argb >> 0), (uint8_t)(argb >> 8), (uint8_t)(argb >> 16)); }
	
	/*!
	 * @brief ピクセルを取得します。
	 * 
	 * @param なし
	 *
	 * @return ピクセルの値をARGB8888形式で返します。
	 */
	uint32_t GetARGB() const{ return (a_ << 24) | (r_ << 16) | (g_ << 8) | (b_ << 0); }

	/*!
	 * @brief ピクセルを取得します。
	 * 
	 * @param なし
	 *
	 * @return ピクセルの値をRGBA8888形式で返します。
	 */
	uint32_t GetRGBA() const { return (r_ << 24) | (a_ << 16) | (b_ << 24) | (g_ << 0); }

	/*!
	 * @brief 値をクリアします。
	 *
	 * @param なし
	 *
	 * @return なし
	 */
	void Clear(){ a_ = 0; r_ = 0; g_ = 0; b_ = 0; }

	/*!
	 * @brief 自身のoperatorです。
	 */
	Color& operator=(const Color& src){
		r_ = src.r_;
		g_ = src.g_;
		b_ = src.b_;
		a_ = src.a_;
		return (*this);
	}

private:
	uint8_t b_;			//!< このピクセルのB値です。
	uint8_t g_;			//!< このピクセルのG値です。
	uint8_t r_;			//!< このピクセルのR値です。
	uint8_t a_;			//!< このピクセルのA値です。
};
