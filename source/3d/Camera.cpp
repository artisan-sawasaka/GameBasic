#include "Camera.h"
#include "utility/DeviceManager.h"
#include "utility/Utility.hpp"
#include "utility/KeyManager.h"

#define SAFE_RELEASE(a) if (a != nullptr) { a->Release(); a = nullptr; }

Camera::Camera()
{
	SetPosition(0.0f, 5.0f, -14);
	SetLookAt(0.0f, 5.0f, 0.0f);
	SetFov(D3DXToRadian(45));
	SetAspect(16.0f / 9.0f);
	SetDepth(0.1f, 1000.0f);
	Apply();
}

Camera::~Camera()
{
}

void Camera::Apply()
{
	D3DXMatrixLookAtLH(&view_, &position_, &lookat_, &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
	D3DXMatrixPerspectiveFovLH(&projection_, fov_, aspect_, znear_, zfar_);
}

void Camera::Update(float df)
{
	auto device = DeviceManager::GetInstance()->GetDevice();
	if (device == nullptr) return ;

	// デバッグ機能
	if (KeyManager::GetInstance()->IsPress(VK_UP)) {
		position_.z += 0.1f;
		Apply();
	} else if (KeyManager::GetInstance()->IsPress(VK_DOWN)) {
		position_.z -= 0.1f;
		Apply();
	}

	device->SetTransform(D3DTS_VIEW, &view_);
	device->SetTransform(D3DTS_PROJECTION, &projection_);
}

void Camera::SetPosition(float x, float y, float z)
{
	position_ = D3DXVECTOR3(x, y, z);
}

void Camera::SetLookAt(float x, float y, float z)
{
	lookat_ = D3DXVECTOR3(x, y, z);
}

void Camera::SetFov(float fov)
{
	fov_ = fov;
}

void Camera::SetAspect(float aspect)
{
	aspect_ = aspect;
}

void Camera::SetDepth(float znear, float zfar)
{
	znear_ = znear;
	zfar_ = zfar;
}

std::string Camera::GetInfo() const
{
	return Utility::StringFormat("px:%5.2f py:%5.2f pz:%5.2f lx:%5.2f ly:%5.2f lz:%5.2f",
		position_.x, position_.y, position_.z,
		lookat_.x, lookat_.y, lookat_.z);
}

