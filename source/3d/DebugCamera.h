#pragma once

#include "Camera.h"

/*!
 * @brief デバッグカメラ
 */
class DebugCamera : public Camera
{
public :
	DebugCamera();
	virtual ~DebugCamera();

	virtual void Update(float df);

private :
	float move_[5];
};
