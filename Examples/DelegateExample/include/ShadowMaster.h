#pragma once

#include "Delegate.h"

DECLARE_MULTICAST_DELEGATE(OnShadowMasterDead)

class ShadowMaster
{
public:

	void Dead()
	{
		shadowMasterDeadDelegate.Broadcast();
	}
	
	OnShadowMasterDead shadowMasterDeadDelegate;
};