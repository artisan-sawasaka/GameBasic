#pragma once

#include <vector>
#include <string>
#include <d3d9.h>
#include <d3dx9.h>

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

private :
	LPD3DXMESH mesh_;
	std::vector<D3DMATERIAL9> materials_;
	std::vector<LPDIRECT3DTEXTURE9> textures_;
};
