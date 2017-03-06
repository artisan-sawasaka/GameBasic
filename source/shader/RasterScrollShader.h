#pragma once

#include "Shader.h"
#include <string>

/*!
 * @brief ラスタースクロール
 */
class RasterScrollShader : public Shader
{
public :
	RasterScrollShader();
	~RasterScrollShader();

	virtual bool LoadFile(const char* path);

private :
};
