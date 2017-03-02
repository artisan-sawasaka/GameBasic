#pragma once

#include <string>
#include <d3dx9.h>

/*!
 * @brief ÉJÉÅÉâ
 */
class Camera
{
public :
	Camera();
	~Camera();

	void Apply();
	void Update(float df);
	void SetPosition(float x, float y, float z);
	void SetLookAt(float x, float y, float z);
	void SetFov(float fovY);
	void SetAspect(float aspect);
	void SetDepth(float znear, float zfar);

	const D3DXVECTOR3& GetPosition() const { return position_; }
	const D3DXVECTOR3& GetLookAt() const { return lookat_; }
	float GetFov() const { return fov_; }
	float GetAspect() const { return aspect_; }
	float GetNear() const { return znear_; }
	float GetFar() const { return zfar_; }

	std::string GetInfo() const;
private :
	D3DXMATRIX view_;
	D3DXMATRIX projection_;
	D3DXVECTOR3 position_;
	D3DXVECTOR3 lookat_;
	float fov_;
	float aspect_;
	float znear_;
	float zfar_;
};
