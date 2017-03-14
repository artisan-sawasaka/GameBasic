#pragma once

#include <vector>
#include <string>
#include <d3dx9.h>
#include <functional>
#include <memory>
#include "render/Texture.h"
#include "render/Color.h"

/*!
 * @brief ƒ‚ƒfƒ‹
 */
class Model
{
public :
	Model();
	virtual ~Model();

	virtual bool LoadFile(const char* path, std::function<std::shared_ptr<Texture>(const char* name)> texture_func = nullptr);
	virtual void Release();
	virtual void Update(float df);
	virtual void Render();

	void SetPotision(const D3DXVECTOR3& position) { position_ = position; }
	void SetPotision(float x, float y, float z) { position_ = D3DXVECTOR3(x, y, z); }
	void SetRotate(const D3DXVECTOR3& rotate) { rotate_ = rotate; }
	void SetRotate(float x, float y, float z) { rotate_ = D3DXVECTOR3(x, y, z); }
	void SetScale(const D3DXVECTOR3& scale) { scale_ = scale; }
	void SetScale(float x, float y, float z) { scale_ = D3DXVECTOR3(x, y, z); }
	void SetColor(const Color& color) { color_ = color; }
	void SetZAlphaEnable(bool enable) { zalpha_enable_ = enable; }

	const D3DXVECTOR3& GetPotision() const { return position_; }
	const D3DXVECTOR3& GetRotate() const { return rotate_; }
	const D3DXVECTOR3& GetScale() const { return scale_; }
	const Color& GetColor() const { return color_; }

protected :
	void CreateWorldMatrix_(D3DXMATRIX& mat);

	LPD3DXMESH mesh_;
	std::vector<D3DMATERIAL9> materials_;
	std::vector<D3DCOLORVALUE> diffuses_;
	std::vector<std::shared_ptr<Texture>> textures_;
	D3DXVECTOR3 position_;
	D3DXVECTOR3 rotate_;
	D3DXVECTOR3 scale_;
	Color color_;
	bool zalpha_enable_;
};
