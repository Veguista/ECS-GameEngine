#include "PoolInitializationClass.h"

#include "Engine/Components/Transform/C_Transform2D.h"
#include "Engine/Components/Rendering/C_TextureRenderer.h"
#include "Engine/Components/Collision/C_Collider2D.h"
#include "Engine/Components/Rigidbody/C_RigidBody2D.h"
#include "Game/Components/C_PlayerController.h"
#include "Game/Components/C_BallController.h"
#include "Game/BubbleSpawner.h"
#include "Game/GameScoreCounter.h"

void PoolInitializationClass::InitializeECSPools(ECS_PoolManager* _PoolManager)
{
	// _PoolManager->CreateEntityPool<>(10, std::string("Systems_Pool"));
	_PoolManager->CreateEntityPool<C_Transform2D, C_TextureRenderer, C_Collider2D, BubbleSpawner, GameScoreCounter>(5, std::string("Background_Pool"));
	_PoolManager->CreateEntityPool<C_Transform2D, C_TextureRenderer, C_Collider2D, C_Rigidbody2D, C_BallController>(100, std::string("Bubble_Pool"));
	_PoolManager->CreateEntityPool<C_Transform2D, C_TextureRenderer, C_PlayerController, C_Collider2D, C_Rigidbody2D>(5, std::string("Player_Pool"));
}
