#include "RasterScrollShader.h"
#include "utility/DeviceManager.h"
#include <string>

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

RasterScrollShader::RasterScrollShader()
{
}

RasterScrollShader::~RasterScrollShader()
{
	Release();
}

bool RasterScrollShader::LoadFile(const char* path)
{
	if (!Shader::LoadFile(path)) {
		return false;
	}

	return true;
}
