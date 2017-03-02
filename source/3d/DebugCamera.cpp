#include "DebugCamera.h"
#include "utility/KeyManager.h"
#include "utility/MouseManager.h"

DebugCamera::DebugCamera()
{
	std::fill(move_, move_ + sizeof(move_) / sizeof(*move_), 0.0f);
}

DebugCamera::~DebugCamera()
{
}

void DebugCamera::Update(float df)
{
	auto* ms = MouseManager::GetInstance();

	// ç¿ïW
	if (ms->IsDrag(MouseManager::LBUTTON)) {
		move_[0] = ms->GetMoveX() / 10.0f;
		move_[1] = -ms->GetMoveY() / 10.0f;
	}
	if (ms->IsHwheelUp()) {
		move_[2] = 2.0f;
	} else if (ms->IsHwheelDown()) {
		move_[2] = -2.0f;
	}
	position_ += D3DXVECTOR3(move_[0], move_[1], move_[2]);

	// å¸Ç´
	if (ms->IsDrag(MouseManager::RBUTTON)) {
		move_[3] = ms->GetMoveX() / 400.0f;
		move_[4] = ms->GetMoveY() / 400.0f;
	}
	rotate_ += D3DXVECTOR3(move_[4], move_[3], 0.0f);

	Apply();

	// äµê´
	for (int i = 0; i < sizeof(move_) / sizeof(*move_); ++i) {
		move_[i] *= 0.8f;
		if (abs(move_[i]) < 0.001f) {
			move_[i] = 0.0f;
		}
	}

	Camera::Update(df);
}
