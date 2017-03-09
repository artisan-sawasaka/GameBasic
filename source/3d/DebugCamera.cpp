#include "DebugCamera.h"
#include "input/Key.h"
#include "input/Mouse.h"

DebugCamera::DebugCamera()
{
	std::fill(move_, move_ + sizeof(move_) / sizeof(*move_), 0.0f);
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(float df)
{
	auto* ms = Mouse::GetInstance();

	// À•W
	if (ms->IsDrag(Mouse::LBUTTON)) {
		move_[0] = ms->GetMoveX() / 10.0f;
		move_[1] = -ms->GetMoveY() / 10.0f;
	}
	if (ms->IsHwheelUp()) {
		if (ms->IsPress(Mouse::RBUTTON)) {
			move_[5] = 0.5f;
		} else {
			move_[2] = 2.0f;
		}
	} else if (ms->IsHwheelDown()) {
		if (ms->IsPress(Mouse::RBUTTON)) {
			move_[5] -= 0.5f;
		} else {
			move_[2] -= 2.0f;
		}
	}
	position_ += D3DXVECTOR3(move_[0], move_[1], move_[2]);

	// Œü‚«
	if (ms->IsDrag(Mouse::RBUTTON)) {
		move_[3] = ms->GetMoveX() / 400.0f;
		move_[4] = ms->GetMoveY() / 400.0f;
	}
	rotate_ += D3DXVECTOR3(move_[4], move_[3], 0.0f);

	// ‹–ìŠp
	fov_ += move_[5];

	if (ms->IsPress(Mouse::LBUTTON) && ms->IsPress(Mouse::RBUTTON)) {
		Initialize_();
	}

	Apply();

	// Šµ«
	for (int i = 0; i < sizeof(move_) / sizeof(*move_); ++i) {
		move_[i] *= 0.8f;
		if (abs(move_[i]) < 0.001f) {
			move_[i] = 0.0f;
		}
	}

	Camera::Update(df);
}
