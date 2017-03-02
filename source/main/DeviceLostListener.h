#pragma once

class DeviceLostListener
{
public :
	virtual void OnLostDevice() = 0;
	virtual void OnResetDevice() = 0;
};
