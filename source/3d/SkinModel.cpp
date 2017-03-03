#include "SkinModel.h"
#include "utility/DeviceManager.h"
#include "utility/Utility.hpp"

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }
static const float RotateBase = 6.28318530718f;
struct OXD3DXFRAME : public D3DXFRAME {
	DWORD id;
	D3DXMATRIX offsetMatrix;
	OXD3DXFRAME() : id(0xffffffff) {
		D3DXMatrixIdentity(&offsetMatrix);
	}
};

struct OXD3DXMESHCONTAINER : public D3DXMESHCONTAINER {
	DWORD maxFaceInfl;
	DWORD numBoneCombinations;
	ID3DXBuffer *boneCombinationTable;
	OXD3DXMESHCONTAINER() : maxFaceInfl(1), numBoneCombinations(0), boneCombinationTable(0) {}
};

class AllocateHierarchy : public ID3DXAllocateHierarchy {
	char *copyName(const char* name) {
		char *n = 0;
		if (!name || name[0] == '\0') {
			n = new char[1];
			n[0] = '\0';
		} else {
			size_t len = strlen(name);
			n = new char[strlen(name) + 1];
			strcpy_s(n, strlen(name) + 1, name);
		}
		return n;
	}

public:
	STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame) {
		OXD3DXFRAME *newFrame = new OXD3DXFRAME;
		newFrame->Name = copyName(Name);
		newFrame->pFrameFirstChild = 0;
		newFrame->pFrameSibling = 0;
		newFrame->pMeshContainer = 0;
		D3DXMatrixIdentity(&newFrame->TransformationMatrix);
		*ppNewFrame = newFrame;

		return D3D_OK;
	}

	STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree) {
		if (pFrameToFree->pFrameFirstChild)
			DestroyFrame(pFrameToFree->pFrameFirstChild);
		if (pFrameToFree->pFrameSibling)
			DestroyFrame(pFrameToFree->pFrameSibling);
		if (pFrameToFree->pMeshContainer)
			DestroyMeshContainer(pFrameToFree->pMeshContainer);

		delete[] pFrameToFree->Name;
		delete pFrameToFree;

		return D3D_OK;
	}

	STDMETHOD(CreateMeshContainer)(THIS_
		LPCSTR Name,
		CONST D3DXMESHDATA *pMeshData,
		CONST D3DXMATERIAL *pMaterials,
		CONST D3DXEFFECTINSTANCE *pEffectInstances,
		DWORD NumMaterials,
		CONST DWORD *pAdjacency,
		LPD3DXSKININFO pSkinInfo,
		LPD3DXMESHCONTAINER *ppNewMeshContainer
		)
	{
		OXD3DXMESHCONTAINER *newCont = new OXD3DXMESHCONTAINER;
		newCont->Name = copyName(Name);

		newCont->pAdjacency = new DWORD[pMeshData->pMesh->GetNumFaces() * 3];
		memset(newCont->pAdjacency, 0, pMeshData->pMesh->GetNumFaces() * 3 * sizeof(DWORD));
		newCont->MeshData.Type = pMeshData->Type;
		pSkinInfo->ConvertToBlendedMesh(
			pMeshData->pMesh, 0, pAdjacency, newCont->pAdjacency, 0, 0, &newCont->maxFaceInfl,
			&newCont->numBoneCombinations, &newCont->boneCombinationTable, &newCont->MeshData.pMesh
		);

		newCont->NumMaterials = NumMaterials;
		newCont->pMaterials = new D3DXMATERIAL[NumMaterials];
		memcpy(newCont->pMaterials, pMaterials, NumMaterials * sizeof(D3DXMATERIAL));

		newCont->pEffects = 0;
		if (pEffectInstances) {
			newCont->pEffects = new D3DXEFFECTINSTANCE;
			newCont->pEffects->pEffectFilename = copyName(pEffectInstances->pEffectFilename);
			newCont->pEffects->NumDefaults = pEffectInstances->NumDefaults;
			newCont->pEffects->pDefaults = new D3DXEFFECTDEFAULT[pEffectInstances->NumDefaults];
			for (DWORD i = 0; i < pEffectInstances->NumDefaults; i++) {
				D3DXEFFECTDEFAULT *src = pEffectInstances->pDefaults + i;
				D3DXEFFECTDEFAULT *dest = newCont->pEffects->pDefaults + i;
				dest->NumBytes = src->NumBytes;
				dest->Type = src->Type;
				dest->pParamName = copyName(src->pParamName);
				dest->pValue = new char[src->NumBytes];
				memcpy(dest->pValue, src->pValue, src->NumBytes);
			}
		}

		newCont->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		*ppNewMeshContainer = newCont;

		return D3D_OK;
	}

	STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerToFree)
	{
		OXD3DXMESHCONTAINER *m = (OXD3DXMESHCONTAINER*)pMeshContainerToFree;

		m->MeshData.pMesh->Release();
		delete[] m->Name;
		delete[] m->pAdjacency;
		if (m->pEffects) {
			for (DWORD i = 0; i < m->pEffects->NumDefaults; i++) {
				D3DXEFFECTDEFAULT *d = m->pEffects->pDefaults + i;
				delete[] d->pParamName;
				delete[] d->pValue;
			}
			delete[] m->pEffects->pDefaults;
			delete[] m->pEffects->pEffectFilename;
			delete m->pEffects;
		}
		delete[] m->pMaterials;

		if (m->pSkinInfo)
			m->pSkinInfo->Release();

		if (m->boneCombinationTable)
			m->boneCombinationTable->Release();

		delete m;

		return D3D_OK;
	}
};


struct SkinData {
	AllocateHierarchy allocater;
	OXD3DXFRAME* pRootFrame;
	ID3DXAnimationController* controller;
	OXD3DXMESHCONTAINER* container;
	SkinData()
		: pRootFrame(nullptr), controller(nullptr), container(nullptr)
	{}

	~SkinData() {
		if (pRootFrame != nullptr) {
			allocater.DestroyFrame(pRootFrame);
		}
		SAFE_RELEASE(controller);
	}
};

OXD3DXMESHCONTAINER *getMeshContainer(D3DXFRAME *frame)
{
	if (frame->pMeshContainer)
		return (OXD3DXMESHCONTAINER*)frame->pMeshContainer;
	if (frame->pFrameFirstChild)
		return getMeshContainer(frame->pFrameFirstChild);
	if (frame->pFrameSibling)
		return getMeshContainer(frame->pFrameSibling);
	return 0;
}

void setFrameId(OXD3DXFRAME *frame, ID3DXSkinInfo *info)
{
	std::map<std::string, DWORD> nameToIdMap;
	for (DWORD i = 0; i < info->GetNumBones(); i++)
		nameToIdMap[info->GetBoneName(i)] = i;

	struct create {
		static void f(std::map<std::string, DWORD> nameToIdMap, ID3DXSkinInfo *info, OXD3DXFRAME* frame)
		{
			if (nameToIdMap.find(frame->Name) != nameToIdMap.end()) {
				frame->id = nameToIdMap[frame->Name];
				frame->offsetMatrix = *info->GetBoneOffsetMatrix(frame->id);
			}
			if (frame->pFrameFirstChild)
				f(nameToIdMap, info, (OXD3DXFRAME*)frame->pFrameFirstChild);
			if (frame->pFrameSibling)
				f(nameToIdMap, info, (OXD3DXFRAME*)frame->pFrameSibling);
		}
	};
	create::f(nameToIdMap, info, frame);
}

void updateCombMatrix(std::map<DWORD, D3DXMATRIX> &combMatrixMap, OXD3DXFRAME *frame)
{
	struct update {
		static void f(std::map<DWORD, D3DXMATRIX> &combMatrixMap, D3DXMATRIX &parentBoneMatrix, OXD3DXFRAME *frame)
		{
			D3DXMATRIX &localBoneMatrix = frame->TransformationMatrix;
			D3DXMATRIX boneMatrix = localBoneMatrix * parentBoneMatrix;
			if (frame->id != 0xffffffff)
				combMatrixMap[frame->id] = frame->offsetMatrix * boneMatrix;
			if (frame->pFrameFirstChild)
				f(combMatrixMap, boneMatrix, (OXD3DXFRAME*)frame->pFrameFirstChild);
			if (frame->pFrameSibling)
				f(combMatrixMap, parentBoneMatrix, (OXD3DXFRAME*)frame->pFrameSibling);
		}
	};
	D3DXMATRIX iden;
	D3DXMatrixIdentity(&iden);
	update::f(combMatrixMap, iden, frame);
}


SkinModel::SkinModel()
{
}

SkinModel::~SkinModel()
{
	Release();
}

bool SkinModel::LoadFile(const char* path, std::function<std::shared_ptr<Texture>(const char* name)> texture_func)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr || path == nullptr) return false;

	Release();

	data_.reset(new SkinData());
	auto ret = D3DXLoadMeshHierarchyFromXA(path, D3DXMESH_MANAGED, device, &data_->allocater, 0, (D3DXFRAME**)&data_->pRootFrame, &data_->controller);
	if (ret != D3D_OK) {
		return false;
	}

	setFrameId(data_->pRootFrame, cont->pSkinInfo);

	return true;
}

void SkinModel::Release()
{
	data_.reset();
}

void SkinModel::Update(float df)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr || mesh_ == nullptr) return;
}
