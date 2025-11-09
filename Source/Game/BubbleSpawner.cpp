#include "BubbleSpawner.h"
#include "Engine/ECS/ECS_PoolManager.h"
#include "Engine/Components/Rigidbody/C_RigidBody2D.h"

void BubbleSpawner::Update(float _DeltaTime)
{
	BubbleSpawnTimer -= _DeltaTime;

	if (BubbleSpawnTimer <= 0)
	{
		BubbleSpawnTimer = MaxTimer;
		int whichBubbleToSpawn = rand() % 3;

		EntityID spawnedEntity;

		if (whichBubbleToSpawn == 0)
		{
			spawnedEntity = BlueBubblePrefab.Instantiate();
		}
		else if (whichBubbleToSpawn == 1)
		{
			spawnedEntity = GreenBubblePrefab.Instantiate();
		}
		else
		{
			spawnedEntity = RedBubblePrefab.Instantiate();
		}

		// If our spawn was successful.
		if (spawnedEntity != ECS::CONSTANTS::InvalidEntityID())
		{
			ECS_PoolManager* manager = ECS_PoolManager::GetInstance();
			ECS_EntityPool* pool = manager->GetEntityPool(ECS::GetPoolFromId(spawnedEntity));

			// Placing the Entity in a random starting position.
			pool->GetComponent<C_Transform2D>(spawnedEntity)->m_pos.x = (rand() % 85) * 5;
			pool->GetComponent<C_Transform2D>(spawnedEntity)->m_pos.y = 70 + (rand() % 20) * 4;

			// Giving more diversity by switching direction of initial X Velocity.
			pool->GetComponent<C_Rigidbody2D>(spawnedEntity)->m_velocity.x *= (rand() % 2) ? 1 : -1;
		}
	}
}
