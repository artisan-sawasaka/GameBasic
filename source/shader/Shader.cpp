#include "Shader.h"
#include "utility/DeviceManager.h"
#include <string>

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

Shader::Shader()
	: effect_(nullptr)
	, color_(Color::White)
{
	DeviceManager::GetInstance()->GetDevice()->AddDeviceLostListener(this);
}

Shader::~Shader()
{
	Release();
	DeviceManager::GetInstance()->GetDevice()->RemoveDeviceLostListener(this);
}

bool Shader::LoadFile(const char* path)
{
	auto device = DeviceManager::GetInstance()->GetDevice()->GetDevice();
	if (device == nullptr || path == nullptr) return false;

	Release();

	//ファイルの読み込み
	LPD3DXBUFFER message;
	HRESULT hr = D3DXCreateEffectFromFileA(device, path, nullptr, nullptr, 0, nullptr, &effect_, &message);
	if (FAILED(hr)) {
		std::string s = static_cast<const char*>(message->GetBufferPointer());
		SAFE_RELEASE(message);
		return false;
	}
	SAFE_RELEASE(message);

	InitializeHandle_();

	return true;
}

void Shader::Release()
{
	SAFE_RELEASE(effect_);
}

void Shader::SetTexture(Texture* texture)
{
	if (effect_ == nullptr) return;

	effect_->SetTexture(src_map_, texture->GetTexture());
}

void Shader::SetColor(const Color& color, float rate)
{
	color_ = color;
	const float col[4] = {
		color.GetR() * rate / 255.0f,
		color.GetG() * rate / 255.0f,
		color.GetB() * rate / 255.0f,
		color.GetA() * rate / 255.0f,
	};
	effect_->SetFloatArray(diffuse_handle_, col, 4);
}

void Shader::Begin(int index)
{
	if (effect_ == nullptr) return;

	effect_->SetTechnique(techniqe_);
	effect_->Begin(nullptr, 0);
	effect_->BeginPass(index);
}

void Shader::End()
{
	if (effect_ == nullptr) return;
	effect_->EndPass();
	effect_->End();
}

void Shader::OnLostDevice()
{
	if (effect_ == nullptr) return;
	effect_->OnLostDevice();
}

void Shader::OnResetDevice()
{
	if (effect_ == nullptr) return;
	effect_->OnResetDevice();
}

void Shader::InitializeHandle_()
{
	techniqe_ = effect_->GetTechniqueByName("TShader");
	src_map_ = effect_->GetParameterByName(nullptr, "SrcMap");
	diffuse_handle_ = effect_->GetParameterByName(nullptr, "Diffuse");
}

bool Shader::Create_(const char* src)
{
	auto device = DeviceManager::GetInstance()->GetDevice()->GetDevice();
	if (device == nullptr) return false;

	//ファイルの読み込み
	LPD3DXBUFFER message;
	HRESULT hr = D3DXCreateEffect(device, src, strlen(src), nullptr, nullptr, 0, nullptr, &effect_, &message);
	if (FAILED(hr)) {
		std::string s = static_cast<const char*>(message->GetBufferPointer());
		SAFE_RELEASE(message);
		return false;
	}
	SAFE_RELEASE(message);
	InitializeHandle_();

	return true;
}
