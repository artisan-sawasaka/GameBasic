#pragma once

#include <windows.h>
#include <d3dx9.h>
#include <cstdint>

class Device
{
public :
	Device();
	~Device();

	bool Initialize(HWND hwnd, uint32_t width, uint32_t height);
	void Finalize();
	bool CheckIdle();
	void CheckDevice();
	LPDIRECT3DDEVICE9 GetDevice() const { return device_; }
	bool IsFullScreen() const { return !screen_window_; }
	void ChangeScreen(bool window);
	void SetBackBufferSize(uint32_t width, uint32_t height);

private :
	void InitRenderState_();

private :
	LPDIRECT3D9 d3d_;
	LPDIRECT3DDEVICE9 device_;
	D3DPRESENT_PARAMETERS d3dpp_;
	D3DPRESENT_PARAMETERS d3dpp_window_;
	D3DPRESENT_PARAMETERS d3dpp_full_;
	HWND hwnd_;
	uint32_t width_;
	uint32_t height_;
	RECT window_rect_;
	bool device_lost_;
	bool screen_window_;
};
