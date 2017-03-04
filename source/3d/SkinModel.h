#pragma once

#include "Model.h"
#include <memory>

struct SkinData;
/*!
 * @brief スキンメッシュのモデル
 */
class SkinModel : public Model
{
public :
	SkinModel();
	~SkinModel();

	virtual bool LoadFile(const char* path, std::function<std::shared_ptr<Texture>(const char* name)> texture_func = nullptr);
	virtual void Release();
	virtual void Update(float df);
	virtual void Render();

private :
	void LoadTexture(const char* name);

	std::shared_ptr<SkinData> data_;
};
