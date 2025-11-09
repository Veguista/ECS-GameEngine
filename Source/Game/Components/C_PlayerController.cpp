#include "C_PlayerController.h"
#include "Engine/ECS/ECS_PoolManager.h"
#include "Engine/ExternalLibraries/Tigr/tigr.h"
#include "Engine/Components/Collision/C_Collider2D.h"
#include "Engine/Components/Rendering/C_TextureRenderer.h"
#include "Game/BubbleSpawner.h"
#include "Game/GameScoreCounter.h"

C_PlayerController::C_PlayerController()
{
	if (Instance != nullptr)
	{
		printf("Can only have one active PlayerController at a time.");
		return;
	}

	Instance = this;

	const ECS_PoolManager* manager = ECS_PoolManager::GetInstance();
	if (manager == nullptr)
	{
		return;
	}

	const Engine* engine = Engine::GetInstance();
	if (engine != nullptr)
	{
		window = engine->GetTigrScreen();
	}

	OwnerEntityId = manager->FindComponentOwnerEntity(this);
	if (OwnerEntityId == ECS::CONSTANTS::InvalidEntityID())
	{
		return;
	}
}

C_PlayerController::~C_PlayerController()
{
	if (Instance != this)
	{
		printf("Can only have one active PlayerController at a time.");
		return;
	}

	Instance = nullptr;
}

void C_PlayerController::MovePlayer(float _DeltaTime)
{
	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	C_Transform2D* playerTransform = poolManager->GetEntityPool(ECS::GetPoolFromId(GetPlayerEntityID()))->GetComponent<C_Transform2D>(GetPlayerEntityID());
	C_Collider2D* playerCollider = poolManager->GetEntityPool(ECS::GetPoolFromId(GetPlayerEntityID()))->GetComponent<C_Collider2D>(GetPlayerEntityID());
	if (playerTransform != nullptr && playerCollider != nullptr)
	{
		playerTransform->m_pos.x += RawHorizontalInput * PLAYER_SPEED * _DeltaTime;

		// Reached the left edge.
		if (playerTransform->m_pos.x + playerCollider->m_Offset.x < playerCollider->m_Dimensions.x / 2)
		{
			playerTransform->m_pos.x = playerCollider->m_Dimensions.x / 2 - playerCollider->m_Offset.x;
		}

		// Reached the right edge.
		if (playerTransform->m_pos.x + playerCollider->m_Offset.x > window->w - playerCollider->m_Dimensions.x / 2)
		{
			playerTransform->m_pos.x = window->w - playerCollider->m_Dimensions.x / 2 - playerCollider->m_Offset.x;
		}
	}
}

void C_PlayerController::Update(float _DeltaTime)
{
	if (window != nullptr)
	{
		// Recording inputs
		int newRawHorizontalInput = 0;
		int newRawVerticalInput = 0;

		// Movement Input.
		if (tigrKeyHeld(window, TK_RIGHT) || tigrKeyHeld(window, 'D'))
		{
			newRawHorizontalInput += 1;
		}
		if (tigrKeyHeld(window, TK_LEFT) || tigrKeyHeld(window, 'A'))
		{
			newRawHorizontalInput += -1;
		}
		if (tigrKeyHeld(window, TK_UP) || tigrKeyHeld(window, 'W'))
		{
			newRawVerticalInput += 1;
		}
		if (tigrKeyHeld(window, TK_DOWN) || tigrKeyHeld(window, 'S'))
		{
			newRawVerticalInput += -1;
		}

		// printf("Player Input = (%i, %i)\n", newRawHorizontalInput, newRawVerticalInput);
		RawHorizontalInput = newRawHorizontalInput;
		RawVerticalInput = newRawVerticalInput;

		// Moving our Player Character.
		if (RunStarted)
		{
			MovePlayer(_DeltaTime);
		}

		// Trying to restart the game.
		if (tigrKeyHeld(window, TK_SPACE))
		{
			TryToRestartGame();
		}
	}
}

void C_PlayerController::TryToRestartGame()
{
	if (RunStarted)
	{
		return;
	}
	
	RunStarted = true;
	printf("Game restarted.\n");

	// Revealing the player.
	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	C_TextureRenderer* playerRenderer = poolManager->GetEntityPool(ECS::GetPoolFromId(GetPlayerEntityID()))->GetComponent<C_TextureRenderer>(GetPlayerEntityID());
	if (playerRenderer != nullptr)
	{
		playerRenderer->SetVisibility(true);
	}
	C_Transform2D* playerTransform = poolManager->GetEntityPool(ECS::GetPoolFromId(GetPlayerEntityID()))->GetComponent<C_Transform2D>(GetPlayerEntityID());
	if (playerTransform != nullptr)
	{
		playerTransform->m_pos.x = 150;
	}

	// Enabling the BubbleSpawner.
	ECS_EntityPool* backgroundPool = poolManager->GetEntityPool(0);
	backgroundPool->AssignComponent<BubbleSpawner>(backgroundPool->m_entities[0].m_id);

	// Stating the Score Counter.
	backgroundPool->GetComponent<GameScoreCounter>(backgroundPool->m_entities[0].m_id)->StartNewRun();
}

void C_PlayerController::TryToEndGame()
{
	if (!RunStarted)
	{
		return;
	}

	RunStarted = false;
	printf("Game ended.\n");
	
	// Hiding the player.
	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	C_TextureRenderer* playerRenderer = poolManager->GetEntityPool(ECS::GetPoolFromId(GetPlayerEntityID()))->GetComponent<C_TextureRenderer>(GetPlayerEntityID());
	if (playerRenderer != nullptr)
	{
		playerRenderer->SetVisibility(false);
	}

	// Disabling the BubbleSpawner.
	ECS_EntityPool* backgroundPool = poolManager->GetEntityPool(0);
	backgroundPool->RemoveComponent<BubbleSpawner>(backgroundPool->m_entities[0].m_id);

	// Destroying all Bubbles.
	ECS_EntityPool* bubblePool = poolManager->GetEntityPool(1);
	bubblePool->DestroyAllEntities();

	// Stopping the Score Counter.
	backgroundPool->GetComponent<GameScoreCounter>(backgroundPool->m_entities[0].m_id)->EndRun();
}
