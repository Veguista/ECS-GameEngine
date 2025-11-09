#pragma once

#include "Engine/ECS_Pools_Init_Base.h"

class PoolInitializationClass : public ECS_Pool_Initializer
{
public:
	virtual void InitializeECSPools(ECS_PoolManager* _PoolManager) override;
};