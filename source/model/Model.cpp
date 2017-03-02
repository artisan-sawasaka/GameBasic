#include "Model.h"
#include "utility/DeviceManager.h"
#include "utility/Utility.hpp"

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

Model::Model()
	: mesh_(nullptr)
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
	auto ret = D3DXLoadMeshFromXA(path, D3DXMESH_IB_MANAGED, device, nullptr, &buffer, nullptr, &material_num, &mesh_);	if (ret != D3D_OK) {
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
	D3DXMATERIAL* d3dxmatrs = static_cast<D3DXMATERIAL*>(buffer->GetBufferPointer());

	for (DWORD i = 0; i < material_num; ++i) {
		//マテリアル複写
		materials_[i] = d3dxmatrs[i].MatD3D;
		materials_[i].Ambient = materials_[i].Diffuse;

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
}

void Model::Render()
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr) return ;

	for (size_t i = 0; i < materials_.size(); ++i) {
		device->SetMaterial(&materials_[i]);
		device->SetTexture(0, textures_[ i ]);
		mesh_->DrawSubset(0);
	}
}
