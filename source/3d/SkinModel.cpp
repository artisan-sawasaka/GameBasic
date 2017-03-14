#include "SkinModel.h"
#include "render/Renderer.h"
#include "utility/DeviceManager.h"
#include "utility/PathUtility.hpp"
#include "utility/KeyUtility.hpp"
#include <map>

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }
static const float PI2 = 6.28318530718f;

// メッシュコンテナ
struct CD3DXMeshContainer : public D3DXMESHCONTAINER {
	DWORD maxFaceInfl;
	DWORD numBoneCombinations;
	ID3DXBuffer* boneCombinationTable;
	CD3DXMeshContainer() : maxFaceInfl(1), numBoneCombinations(0), boneCombinationTable(0) {}
};

// フレーム
struct CD3DXFrame : public D3DXFRAME {
	DWORD id;
	D3DXMATRIX offsetMatrix;
	CD3DXFrame() : id(0xffffffff) {
		D3DXMatrixIdentity(&offsetMatrix);
	}

	// コンテナを取得
	CD3DXMeshContainer* GetMeshContainer() {
		return GetMeshContainer_(this);
	}

	// フレームIDを設定
	void InitializeFrameId() {
		ID3DXSkinInfo* info = GetMeshContainer()->pSkinInfo;
		if (info == nullptr) return ;

		std::map<std::string, DWORD> nameToIdMap;
		for (DWORD i = 0; i < info->GetNumBones(); ++i) {
			nameToIdMap[info->GetBoneName(i)] = i;
		}

		SetFrameIdAndMatrix_(nameToIdMap, info, this);
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		UpdateCombMatrix(mat);
	}

	// マトリクスを更新
	void UpdateCombMatrix(D3DXMATRIX& mat)	{
		UpdateCombMatrix_(mat, this);
	}

	// 指定されたIDのマトリクスを取得
	const D3DXMATRIX* GetMatrix(int id) const {
		auto it = combMatrixMap_.find(id);
		if (it != combMatrixMap_.end()) {
			return &it->second;
		}
		return nullptr;
	}

private :
	// フレームの階層を辿ってコンテナを取得
	CD3DXMeshContainer* GetMeshContainer_(D3DXFRAME* frame) {
		if (frame->pMeshContainer != nullptr)
			return static_cast<CD3DXMeshContainer*>(frame->pMeshContainer);
		if (frame->pFrameFirstChild != nullptr)
			return GetMeshContainer_(frame->pFrameFirstChild);
		if (frame->pFrameSibling != nullptr)
			return GetMeshContainer_(frame->pFrameSibling);
		return 0;
	}

	// 各フレームにIDを付与する
	void SetFrameIdAndMatrix_(std::map<std::string, DWORD>& nameToIdMap, ID3DXSkinInfo* info, CD3DXFrame* frame) {
		if (nameToIdMap.find(frame->Name) != nameToIdMap.end()) {
			frame->id = nameToIdMap[frame->Name];
			frame->offsetMatrix = *info->GetBoneOffsetMatrix(frame->id);
		}
		if (frame->pFrameFirstChild) {
			SetFrameIdAndMatrix_(nameToIdMap, info, static_cast<CD3DXFrame*>(frame->pFrameFirstChild));
		}
		if (frame->pFrameSibling) {
			SetFrameIdAndMatrix_(nameToIdMap, info, static_cast<CD3DXFrame*>(frame->pFrameSibling));
		}
	}

	// マトリクスを更新する
	void UpdateCombMatrix_(D3DXMATRIX& parentBoneMatrix, CD3DXFrame* frame) {
		D3DXMATRIX boneMatrix = frame->TransformationMatrix * parentBoneMatrix;
		if (frame->id != 0xffffffff) {
			combMatrixMap_[frame->id] = frame->offsetMatrix * boneMatrix;
		}
		if (frame->pFrameFirstChild) {
			UpdateCombMatrix_(boneMatrix, static_cast<CD3DXFrame*>(frame->pFrameFirstChild));
		}
		if (frame->pFrameSibling) {
			UpdateCombMatrix_(parentBoneMatrix, static_cast<CD3DXFrame*>(frame->pFrameSibling));
		}
	}

	std::map<DWORD, D3DXMATRIX> combMatrixMap_;
};

// 階層とメモリ管理
class AllocateHierarchy : public ID3DXAllocateHierarchy {
public:
	AllocateHierarchy(
		std::vector<D3DMATERIAL9>& materials,
		std::vector<std::shared_ptr<Texture>>& textures,
		std::function<std::shared_ptr<Texture>(const char* name)> texture_func,
		const char* path)
		: materials_(materials)
		, textures_(textures)
		, texture_func_(texture_func)
		, path_(path)
	{}

	// フレーム作成
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME* ppNewFrame) {
		CD3DXFrame* frame = new CD3DXFrame();
		frame->Name = CopyName(Name);
		frame->pFrameFirstChild = nullptr;
		frame->pFrameSibling = nullptr;
		frame->pMeshContainer = nullptr;
		D3DXMatrixIdentity(&frame->TransformationMatrix);
		*ppNewFrame = frame;

		return D3D_OK;
	}

	// メッシュコンテナ作成
	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA* pMeshData,
		CONST D3DXMATERIAL* pMaterials,
		CONST D3DXEFFECTINSTANCE* pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD* pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER* ppNewMeshContainer
		)
	{
		// 新しいメッシュコンテナを作成
		CD3DXMeshContainer *newCont = new CD3DXMeshContainer();
		newCont->Name = CopyName(Name);

		// 各種必要な項目をコピー
		newCont->pAdjacency = new DWORD[pMeshData->pMesh->GetNumFaces() * 3];
		memset(newCont->pAdjacency, 0, pMeshData->pMesh->GetNumFaces() * 3 * sizeof(DWORD));
		newCont->MeshData.Type = pMeshData->Type;
		pSkinInfo->ConvertToBlendedMesh(
			pMeshData->pMesh, 0, pAdjacency, newCont->pAdjacency, 0, 0, &newCont->maxFaceInfl,
			&newCont->numBoneCombinations, &newCont->boneCombinationTable, &newCont->MeshData.pMesh
		);

		// マテリアルのコピーとテクスチャーのロード
		materials_.resize(NumMaterials);
		textures_.resize(NumMaterials);
		for (DWORD i = 0; i < NumMaterials; ++i) {
			//マテリアル複写
			materials_[i] = pMaterials[i].MatD3D;

			//テクスチャをロード
			textures_[i].reset(new Texture());
			const char* texture_path = pMaterials[i].pTextureFilename;
			if (texture_path == nullptr)
				continue;

			auto name = PathUtility::GetFileName(texture_path);
			if (texture_func_) {
				// 指定テクスチャーロード関数の呼び出し
				textures_[i] = texture_func_(name.c_str());
			} else {
				// 指定がない場合は同一ディレクトリにテクスチャーがあると判断
				auto dir = PathUtility::GetDirectoryName(path_.c_str());
				auto tex_path = PathUtility::Combine(dir, name);

				textures_[i].reset(new Texture());
				textures_[i]->CreateFromFile(tex_path.c_str());
			}
		}

		// エフェクト作成
		newCont->pEffects = nullptr;
		if (pEffectInstances != nullptr) {
			newCont->pEffects = new D3DXEFFECTINSTANCE();
			newCont->pEffects->pEffectFilename = CopyName(pEffectInstances->pEffectFilename);
			newCont->pEffects->NumDefaults = pEffectInstances->NumDefaults;
			newCont->pEffects->pDefaults = new D3DXEFFECTDEFAULT[pEffectInstances->NumDefaults];
			for (DWORD i = 0; i < pEffectInstances->NumDefaults; ++i) {
				D3DXEFFECTDEFAULT *src = pEffectInstances->pDefaults + i;
				D3DXEFFECTDEFAULT *dest = newCont->pEffects->pDefaults + i;
				dest->Type = src->Type;
				dest->pParamName = CopyName(src->pParamName);
				dest->pValue = new char[src->NumBytes];
				dest->NumBytes = src->NumBytes;
				memcpy(dest->pValue, src->pValue, src->NumBytes);
			}
		}

		// スキン情報を保持
		newCont->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		// 作成したコンテナを渡す
		*ppNewMeshContainer = newCont;

		return D3D_OK;
	}

	// フレーム削除
	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) {
		if (pFrameToFree->pFrameFirstChild != nullptr) {
			DestroyFrame(pFrameToFree->pFrameFirstChild);
			pFrameToFree->pFrameFirstChild = nullptr;
		}
		if (pFrameToFree->pFrameSibling != nullptr) {
			DestroyFrame(pFrameToFree->pFrameSibling);
			pFrameToFree->pFrameSibling = nullptr;
		}

		delete[] pFrameToFree->Name;
		delete pFrameToFree;

		return D3D_OK;
	}

	// コンテナ削除
	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
	{
		CD3DXMeshContainer *m = static_cast<CD3DXMeshContainer*>(pMeshContainerToFree);

		SAFE_RELEASE(m->MeshData.pMesh);
		delete[] m->Name;
		delete[] m->pAdjacency;

		if (m->pEffects != nullptr) {
			for (DWORD i = 0; i < m->pEffects->NumDefaults; ++i) {
				D3DXEFFECTDEFAULT *d = m->pEffects->pDefaults + i;
				delete[] d->pParamName;
				delete[] d->pValue;
			}
			delete[] m->pEffects->pDefaults;
			delete[] m->pEffects->pEffectFilename;
			delete m->pEffects;
		}

		SAFE_RELEASE(m->pSkinInfo);
		SAFE_RELEASE(m->boneCombinationTable);

		delete m;

		return D3D_OK;
	}
private :
	char* CopyName(const char* name) {
		char* s;
		if (name == nullptr) {
			s = new char[1];
			s[0] = '\0';
		} else {
			s = new char[strlen(name) + 1];
			strcpy(s, name);
		}
		return s;
	}

	std::vector<D3DMATERIAL9>& materials_;
	std::vector<std::shared_ptr<Texture>>& textures_;
	std::function<std::shared_ptr<Texture>(const char* name)> texture_func_;
	std::string path_;
};

struct SkinData {
	CD3DXFrame* frame;
	std::shared_ptr<AllocateHierarchy> allocater;
	ID3DXAnimationController* controller;
	SkinData()
		: frame(nullptr), controller(nullptr)
	{}

	~SkinData() {
		if (frame != nullptr) {
			D3DXFrameDestroy(frame, allocater.get());
		}
		allocater.reset();
		SAFE_RELEASE(controller);
	}
};


// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SkinModel::SkinModel()
{
}

SkinModel::~SkinModel()
{
	Release();
}

bool SkinModel::LoadFile(const char* path, std::function<std::shared_ptr<Texture>(const char* name)> texture_func)
{
	auto device = DeviceManager::GetInstance()->GetDevice()->GetDevice();
	if (device == nullptr || path == nullptr) return false;

	Release();

	data_.reset(new SkinData());
	data_->allocater.reset(new AllocateHierarchy(materials_, textures_, texture_func, path));
	auto ret = D3DXLoadMeshHierarchyFromXA(path, D3DXMESH_MANAGED, device, data_->allocater.get(), nullptr, (D3DXFRAME**)&data_->frame, &data_->controller);
	if (ret != D3D_OK) {
		Release();
		return false;
	}

	// フレーム初期化
	data_->frame->InitializeFrameId();

	// ディフーズ初期化
	diffuses_.resize(materials_.size());
	for (size_t i = 0; i < materials_.size(); ++i) {
		diffuses_[i] = materials_[i].Diffuse;
	}

	return true;
}

void SkinModel::Release()
{
	data_.reset();
	Model::Release();
}

void SkinModel::Update(float df)
{
	if (!data_) return ;

	D3DXMATRIX mat;
	CreateWorldMatrix_(mat);

	data_->controller->AdvanceTime(df, nullptr);
	data_->frame->UpdateCombMatrix(mat);
}

void SkinModel::Render()
{
	auto device = DeviceManager::GetInstance()->GetDevice()->GetDevice();
	if (device == nullptr || !data_ || color_.GetA() == 0) return ;

	// アルファブレンド
	Renderer::GetInstance()->SetBlend(Renderer::BLEND_ALPHA);
	float r = color_.GetR() / 255.0f;
	float g = color_.GetG() / 255.0f;
	float b = color_.GetB() / 255.0f;
	float a = color_.GetA() / 255.0f;
	for (size_t i = 0; i < materials_.size(); ++i) {
		auto& dst_diff = materials_[i].Diffuse;
		auto& src_diff = diffuses_[i];
		dst_diff.r = src_diff.r * r;
		dst_diff.g = src_diff.g * g;
		dst_diff.b = src_diff.b * b;
		dst_diff.a = src_diff.a * a;
	}

	// 頂点
	auto* frame = data_->frame;
	auto* container = frame->GetMeshContainer();
	auto* mesh = container->MeshData.pMesh;
	auto* combs = static_cast<D3DXBONECOMBINATION*>(container->boneCombinationTable->GetBufferPointer());
	device->SetVertexShader(nullptr);
	device->SetFVF(mesh->GetFVF());

	// 描画
	if (a < 1.0f && zalpha_enable_) {
		// 半透明あり
		Renderer::GetInstance()->SetBlend(Renderer::BLEND_DEST);
		device->SetTexture(0, nullptr);
		for (DWORD i = 0; i < container->numBoneCombinations; ++i) {
			DWORD boneNum = 0;
			for (DWORD j = 0; j < container->maxFaceInfl; ++j) {
				DWORD id = combs[i].BoneId[j];
				if (id != UINT_MAX) {
					device->SetTransform(D3DTS_WORLDMATRIX(j), frame->GetMatrix(id));
					boneNum = j;
				}
			}
			int index = combs[i].AttribId;
			device->SetRenderState(D3DRS_VERTEXBLEND, boneNum);
			mesh->DrawSubset(i);
		}
		Renderer::GetInstance()->SetBlend(Renderer::BLEND_ALPHA);
		for (DWORD i = 0; i < container->numBoneCombinations; ++i) {
			DWORD boneNum = 0;
			for (DWORD j = 0; j < container->maxFaceInfl; ++j) {
				DWORD id = combs[i].BoneId[j];
				if (id != UINT_MAX) {
					device->SetTransform(D3DTS_WORLDMATRIX(j), frame->GetMatrix(id));
					boneNum = j;
				}
			}
			int index = combs[i].AttribId;
			device->SetRenderState(D3DRS_VERTEXBLEND, boneNum);
			device->SetMaterial(&materials_[index]);
			textures_[index]->Apply();
			mesh->DrawSubset(i);
		}
	} else {
		// 半透明なし
		for (DWORD i = 0; i < container->numBoneCombinations; ++i) {
			DWORD boneNum = 0;
			for (DWORD j = 0; j < container->maxFaceInfl; ++j) {
				DWORD id = combs[i].BoneId[j];
				if (id != UINT_MAX) {
					device->SetTransform(D3DTS_WORLDMATRIX(j), frame->GetMatrix(id));
					boneNum = j;
				}
			}
			int index = combs[i].AttribId;
			device->SetRenderState(D3DRS_VERTEXBLEND, boneNum);
			device->SetMaterial(&materials_[index]);
			textures_[index]->Apply();
			mesh->DrawSubset(i);
		}
	}
}
