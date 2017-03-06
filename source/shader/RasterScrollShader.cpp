#include "RasterScrollShader.h"
#include "utility/DeviceManager.h"
#include <string>

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

static const char* fx = {
	"// -------------------------------------------------------------\n"
	"// テクスチャ\n"
	"// -------------------------------------------------------------\n"
	"texture SrcMap;\n"
	"float Offset = 0.0f;\n"
	"float Period = 0.0f;\n"
	"float Amplitude = 0.24f;\n"
	"\n"
	"sampler SrcSamp = sampler_state\n"
	"{\n"
	"    Texture = <SrcMap>;\n"
	"    MinFilter = LINEAR;\n"
	"    MagFilter = LINEAR;\n"
	"    MipFilter = NONE;\n"
	"\n"
	"    AddressU = BORDER;\n"
	"    AddressV = BORDER;\n"
	"};\n"
	"\n"
	"// -------------------------------------------------------------\n"
	"// ピクセルシェーダプログラム\n"
	"// -------------------------------------------------------------\n"
	"float4 PS_pass1(float2 uv : TEXCOORD0) : COLOR\n"
	"{\n"
	"    float2 dst;\n"
	"    dst.x = uv.x + sin((uv.y + Offset) * Period) * Amplitude;\n"
	"    dst.y = uv.y;\n"
	"\n"
	"    float4 Color = tex2D(SrcSamp, dst);\n"
	"    return Color;\n"
	"}\n"
	"\n"
	"// -------------------------------------------------------------\n"
	"// テクニック\n"
	"// -------------------------------------------------------------\n"
	"technique TShader\n"
	"{\n"
	"    pass P0\n"
	"    {\n"
	"        // シェーダ\n"
	"        PixelShader  = compile ps_2_0 PS_pass1();\n"
	"        \n"
	"        Sampler[0] = (SrcSamp);\n"
	"    }\n"
	"}\n"
};

RasterScrollShader::RasterScrollShader()
{
	auto device = DeviceManager::GetInstance()->GetDevice()->GetDevice();
	if (device == nullptr) return ;

	//ファイルの読み込み
	LPD3DXBUFFER message;
	HRESULT hr = D3DXCreateEffect(device, fx, strlen(fx), nullptr, nullptr, 0, nullptr, &effect_, &message);
	if (FAILED(hr)) {
		std::string s = static_cast<const char*>(message->GetBufferPointer());
		SAFE_RELEASE(message);
		return;
	}
	SAFE_RELEASE(message);
	techniqe_ = effect_->GetTechniqueByName("TShader");
	offset_handle_ = effect_->GetParameterByName(nullptr, "Offset");
	amplitude_handle_ = effect_->GetParameterByName(nullptr, "Amplitude");
	period_handle_ = effect_->GetParameterByName(nullptr, "Period");

	effect_->GetFloat(offset_handle_, &offset_);
	effect_->GetFloat(amplitude_handle_, &amplitude_);
	effect_->GetFloat(period_handle_, &period_);
}

RasterScrollShader::~RasterScrollShader()
{
	Release();
}

void RasterScrollShader::SetOffset(float value)
{
	offset_ = value;
	effect_->SetFloat(offset_handle_, value);
}

void RasterScrollShader::SetAmplitude(float value)
{
	amplitude_ = value;
	effect_->SetFloat(amplitude_handle_, value);
}

void RasterScrollShader::SetPeriod(float value)
{
	period_ = value;
	effect_->SetFloat(period_handle_, value);
}
