#include "Engine.h"
#include "Engine/ECS_Pools_Init_Base.h"
#include "Engine/Components/Rigidbody/C_RigidBody2D.h"
#include "Engine/Components/Collision/C_Collider2D.h"
#include "Game/Components/C_BallController.h"
#include "Game/Components/C_PlayerController.h"
#include "ExternalLibraries/Tigr/tigr.h"


#pragma region ENGINE CONTROLS

bool Engine::Init(ECS_Pool_Initializer* _PoolInitializerClass)
{
	if (Instance != nullptr)
	{
		printf("ERROR : Trying to initialize the Engine when a previous instance of it is still running.");
		return false;
	}

	m_pScreen = tigrWindow(480, 360, "Hello", 0);

	if (m_pScreen == nullptr)
	{
		return false;
	}

	m_isRunning = true;
	m_pPoolManager = ECS_PoolManager::InitManager();

	_PoolInitializerClass->InitializeECSPools(m_pPoolManager);
	Instance = this;

	return true;
}
bool Engine::Input()
{
	return true;
}
bool Engine::UpdateTigrScreen()
{
	tigrUpdate(m_pScreen);
	return true;
}
bool Engine::UpdateDeltaTime()
{
	unscaledDeltaTime = tigrTime() + m_additionalTimeSpentAfterFrame;
	m_additionalTimeSpentAfterFrame = 0;

	// We ensure that deltaTime will never be 0.
	if (unscaledDeltaTime <= 0)
	{
		unscaledDeltaTime = 0.02f;
	}

	deltaTime = unscaledDeltaTime * m_timeScale;

	return true;
}
float Engine::CalculateUnscaledDeltaTimeSinceBeginningOfFrame()
{
	m_additionalTimeSpentAfterFrame += tigrTime();
	return m_additionalTimeSpentAfterFrame;
}
void Engine::UpdatePhysics()
{
	// Rigidbody Physics
	{
		ECS_PoolManager::Iterator iteratorEnd = m_pPoolManager->EndIterator<C_Transform2D, C_Rigidbody2D>();
		for (ECS_PoolManager::Iterator it = m_pPoolManager->BeginIterator<C_Transform2D, C_Rigidbody2D>(); it != iteratorEnd; ++it)
		{
			it.GetComponent<C_Rigidbody2D>()->UpdatePhysics(*it.GetComponent<C_Transform2D>(), GetDeltaTime());
		}
	}

	// Ball Collisions with the edges of the screen and the Player.
	{
		EntityID playerEntityID = ECS::CONSTANTS::InvalidEntityID();
		C_Collider2D* playerCollider = nullptr;
		C_Transform2D* playerTransform = nullptr;
		if (C_PlayerController* playerController = C_PlayerController::GetInstance())
		{
			playerEntityID = playerController->GetPlayerEntityID();
			if (playerEntityID != ECS::CONSTANTS::InvalidEntityID())
			{
				playerCollider = m_pPoolManager->GetEntityPool(ECS::GetPoolFromId(playerEntityID))->GetComponent<C_Collider2D>(playerEntityID);
				playerTransform = m_pPoolManager->GetEntityPool(ECS::GetPoolFromId(playerEntityID))->GetComponent<C_Transform2D>(playerEntityID);
			}
		}

		// Iterating all balls.
		ECS_PoolManager::Iterator ballIteratorEnd = m_pPoolManager->EndIterator<C_Transform2D, C_Collider2D, C_BallController, C_Rigidbody2D>();
		for (ECS_PoolManager::Iterator ballIt = m_pPoolManager->BeginIterator<C_Transform2D, C_Collider2D, C_BallController, C_Rigidbody2D>(); ballIt != ballIteratorEnd; ++ballIt)
		{
			C_Collider2D* ballCollider = ballIt.GetComponent<C_Collider2D>();
			C_Rigidbody2D* ballRigidbody = ballIt.GetComponent<C_Rigidbody2D>();

			// Checking against the edges of the screen.
			vec2& ballPos = ballIt.GetComponent<C_Transform2D>()->m_pos;

			// Left or Right edges
			if (ballRigidbody->m_velocity.x < 0 && ballPos.x + ballCollider->m_Offset.x < ballCollider->m_Dimensions.x / 2 
				|| ballRigidbody->m_velocity.x > 0 && ballPos.x + ballCollider->m_Offset.x >(m_pScreen->w - ballCollider->m_Dimensions.x / 2))
			{
				ballRigidbody->m_velocity.x *= -1;
			}
			// Up or Down edges
			if (ballRigidbody->m_velocity.y < 0 && ballPos.y + ballCollider->m_Offset.y < ballCollider->m_Dimensions.y / 2 
				|| ballRigidbody->m_velocity.y > 0 && ballPos.y + ballCollider->m_Offset.y >(m_pScreen->h - ballCollider->m_Dimensions.y / 2))
			{
				ballRigidbody->m_velocity.y *= -1;
			}

			// Checking against the player.
			if (playerCollider != nullptr && playerTransform != nullptr &&
				ballCollider->CheckOverlap(*ballIt.GetComponent<C_Transform2D>(), *playerCollider, *playerTransform))
			{
				C_PlayerController::GetInstance()->TryToEndGame();
				break;
			}
		}
	}
}
bool Engine::UpdateLogic()
{
	m_pPoolManager->UpdateComponents(GetDeltaTime());
	return true;
}
bool Engine::ClearScreen()
{
	tigrClear(m_pScreen, tigrRGB(0, 0, 0));
	return true;
}
bool Engine::Quit()
{
	tigrFree(m_pScreen);
	m_pScreen = nullptr;

	ECS_PoolManager::DestroyInstance();
	Instance = nullptr;

	return true;
}

#pragma endregion

#pragma region ENGINE FUNCTIONS

bool Engine::ShouldClose()
{
	if (tigrClosed(m_pScreen) || Instance != this)
	{
		m_isRunning = false;
	}

	return m_isRunning;
}
void Engine::Print(const char* text)
{
	tigrPrint(m_pScreen, tfont, 0, 0, tigrRGB(0xff, 0xff, 0xff), text);
}
void Engine::Wait(float _secs)
{
	float timePassed = 0;
	while ((timePassed += tigrTime()) < _secs)
	{

	}

	m_additionalTimeSpentAfterFrame += _secs;
}

#pragma endregion
