#include "Model.h"
#include "utility/DeviceManager.h"
#include "utility/Utility.hpp"
#include "utility/KeyManager.h"

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

Model::Model()
	: mesh_(nullptr)
	, color_(Color::White)
	, position_(0.0f, 0.0f, 0.0f)
	, rotate_(0.0f, 0.0f, 0.0f)
	, scale_(1.0f, 1.0f, 1.0f)
{
}

Model::~Model()
{
	Release();
}

bool Model::LoadFile(const char* path)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr || path == nullptr) return false;

	Release();

	//Xファイルロード
	DWORD material_num;
	LPD3DXBUFFER buffer;
	auto ret = D3DXLoadMeshFromXA(path, D3DXMESH_IB_MANAGED, device, nullptr, &buffer, nullptr, &material_num, &mesh_);
	if (ret != D3D_OK) {
		return false;
	}
	
	//Xファイルに法線がない場合は、法線を書き込む
	if ((mesh_->GetFVF() & D3DFVF_NORMAL) == 0) {
		ID3DXMesh* mesh = NULL;
		mesh_->CloneMeshFVF(mesh_->GetOptions(), mesh_->GetFVF() | D3DFVF_NORMAL, device, &mesh );
		D3DXComputeNormals(mesh, NULL);
		mesh_->Release();
		mesh_ = mesh;
	}
	
	//マテリアル、テクスチャの準備
	materials_.resize(material_num);
	textures_.resize(material_num);
	diffuses_.resize(material_num);
	D3DXMATERIAL* d3dxmatrs = static_cast<D3DXMATERIAL*>(buffer->GetBufferPointer());

	for (DWORD i = 0; i < material_num; ++i) {
		//マテリアル複写
		materials_[i] = d3dxmatrs[i].MatD3D;
		materials_[i].Ambient = materials_[i].Diffuse;
		diffuses_[i] = materials_[i].Diffuse;

		//テクスチャをロード
		textures_[i] = nullptr;
		if (d3dxmatrs[i].pTextureFilename == nullptr)
			continue ;

		auto dir = Utility::GetDirectoryName(path);
		auto name = Utility::GetFileName(d3dxmatrs[i].pTextureFilename);
		auto tex_path = Utility::StringFormat("%s/%s", dir.c_str(), name.c_str());

		if (S_OK != D3DXCreateTextureFromFileA(device, tex_path.c_str(), &textures_[i])) {
			textures_[i] = nullptr;
		}
	}
	buffer->Release();

	return true;
}

void Model::Release()
{
	for (size_t i = 0; i < textures_.size(); ++i) {
		SAFE_RELEASE(textures_[i]);
	}
	std::vector<LPDIRECT3DTEXTURE9>().swap(textures_);
	std::vector<D3DMATERIAL9>().swap(materials_);
	SAFE_RELEASE(mesh_);
}

void Model::Update(float df)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr || mesh_ == nullptr) return;
}

void Model::Render()
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr || mesh_ == nullptr) return ;

	D3DXMATRIX mat, matt;
	D3DXMatrixIdentity(&mat);

	// 拡縮
	if (scale_ != D3DXVECTOR3(1.0f, 1.0f, 1.0f)) {
		mat *= *D3DXMatrixScaling(&matt, scale_.x, scale_.y, scale_.z);
	}
	// Y回転
	if (rotate_.y != 0) {
		mat *= *D3DXMatrixRotationY(&matt, rotate_.y);
	}
	// X回転
	if (rotate_.x != 0) {
		mat *= *D3DXMatrixRotationX(&matt, rotate_.x);
	}
	// Z回転
	if (rotate_.z != 0) {
		mat *= *D3DXMatrixRotationZ(&matt, rotate_.z);
	}
	// 移動
	mat.m[3][0] += position_.x;
	mat.m[3][1] += position_.y;
	mat.m[3][2] += position_.z;
	device->SetTransform(D3DTS_WORLD, &mat);

	// アルファブレンド
	Renderer::GetInstance()->SetBlend(Renderer::BLEND_ALPHA);

	device->SetVertexShader(nullptr);
	device->SetFVF(mesh_->GetFVF());

	/*
	static bool hoge = false;
	if (KeyManager::GetInstance()->IsTrg('L')) {
		hoge = !hoge;
	}
	*/

	float r = color_.GetR() / 255.0f;
	float g = color_.GetG() / 255.0f;
	float b = color_.GetB() / 255.0f;
	float a = color_.GetA() / 255.0f;
	if (a < 1.0f) {
		// 半透明あり
		device->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
		for (size_t i = 0; i < materials_.size(); ++i) {
			materials_[i].Diffuse.a = 0;
			device->SetMaterial(&materials_[i]);
			device->SetTexture(0, nullptr);
			mesh_->DrawSubset(i);
		}
		for (size_t i = 0; i < materials_.size(); ++i) {
			auto& dst_diff = materials_[i].Diffuse;
			auto& src_diff = diffuses_[i];
			dst_diff.r = src_diff.r * r;
			dst_diff.g = src_diff.g * g;
			dst_diff.b = src_diff.b * b;
			dst_diff.a = src_diff.a * a;

			device->SetMaterial(&materials_[i]);
			device->SetTexture(0, textures_[i]);
			mesh_->DrawSubset(i);
		}
	} else {
		// 半透明なし
		for (size_t i = 0; i < materials_.size(); ++i) {
			auto& dst_diff = materials_[i].Diffuse;
			auto& src_diff = diffuses_[i];
			dst_diff.r = src_diff.r * r;
			dst_diff.g = src_diff.g * g;
			dst_diff.b = src_diff.b * b;
			dst_diff.a = src_diff.a * a;

			device->SetMaterial(&materials_[i]);
			device->SetTexture(0, textures_[i]);
			mesh_->DrawSubset(i);
		}
	}
}
