#pragma once

#include <vector>
#include <string>
#include <d3dx9.h>
#include "render/Color.h"

/*!
 * @brief ƒ‚ƒfƒ‹
 */
class Model
{
public :
	Model();
	~Model();

	bool LoadFile(const char* path);
	void Release();

	void Update(float df);
	void Render();

	void SetPotision(const D3DXVECTOR3& position) { position_ = position; }
	void SetPotision(float x, float y, float z) { position_ = D3DXVECTOR3(x, y, z); }
	void SetRotate(const D3DXVECTOR3& rotate) { rotate_ = rotate; }
	void SetRotate(float x, float y, float z) { rotate_ = D3DXVECTOR3(x, y, z); }
	void SetScale(const D3DXVECTOR3& scale) { scale_ = scale; }
	void SetScale(float x, float y, float z) { scale_ = D3DXVECTOR3(x, y, z); }
	void SetColor(const Color& color) { color_ = color; }

	const D3DXVECTOR3& GetPotision() const { return position_; }
	const D3DXVECTOR3& GetRotate() const { return rotate_; }
	const D3DXVECTOR3& GetScale() const { return scale_; }
	const Color& GetColor() const { return color_; }

private :
	LPD3DXMESH mesh_;
	std::vector<D3DMATERIAL9> materials_;
	std::vector<D3DCOLORVALUE> diffuses_;
	std::vector<LPDIRECT3DTEXTURE9> textures_;
	D3DXVECTOR3 position_;
	D3DXVECTOR3 rotate_;
	D3DXVECTOR3 scale_;
	Color color_;
};
