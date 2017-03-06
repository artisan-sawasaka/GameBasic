#include "Shader.h"
#include "utility/DeviceManager.h"
#include <string>

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

Shader::Shader()
	: effect_(nullptr)
	, texture_(nullptr)
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

	//ƒtƒ@ƒCƒ‹‚Ì“Ç‚İ‚İ
	LPD3DXBUFFER message;
	HRESULT hr = D3DXCreateEffectFromFileA(device, path, nullptr, nullptr, 0, nullptr, &effect_, &message);
	if (FAILED(hr)) {
		std::string s = static_cast<const char*>(message->GetBufferPointer());
		SAFE_RELEASE(message);
		return false;
	}
	SAFE_RELEASE(message);
	techniqe_ = effect_->GetTechniqueByName("TShader");

	return true;
}

void Shader::Release()
{
	SAFE_RELEASE(effect_);
}

void Shader::SetTexture(Texture* texture)
{
	texture_ = texture;
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

