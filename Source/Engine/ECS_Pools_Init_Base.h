#pragma once

#include "Engine/ECS/ECS_PoolManager.h"

class ECS_Pool_Initializer
{
public:
	virtual void InitializeECSPools(ECS_PoolManager* _PoolManager) = 0;
};