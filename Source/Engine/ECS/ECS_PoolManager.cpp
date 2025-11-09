#include "ECS_PoolManager.h"
#include "ECS_SupportingFunctions.h"

ECS_PoolManager::ECS_PoolManager()
{
	static bool firstTimeCreated{ false };

	assert(firstTimeCreated == false && "Trying to create an ECS_PoolManager more than once. Never destroy the ECS_PoolManager unless the program is shutting down.");
}

#pragma region Manager General Methods 

ECS_PoolManager* ECS_PoolManager::InitManager()
{
	Instance = new ECS_PoolManager();

	Instance->PoolInfoDocument.append_child("PoolList");

	return Instance;
}

void ECS_PoolManager::DestroyInstance()
{
	if (Instance)
	{
		delete Instance;
		Instance = nullptr;
	}
}

#pragma endregion

#pragma region Entity Management

void ECS_PoolManager::DestroyEntities(EntityID* _entityIdBuffer, unsigned int _lengthOfBuffer)
{
	for (unsigned int i = 0; i < _lengthOfBuffer; i++)
	{
		DestroyEntity(_entityIdBuffer[i]);
	}
}

#pragma endregion

#pragma region T Management

void ECS_PoolManager::UpdateComponents(float _deltaTime)
{
	if (!m_IUpdateComponentIds.any())
	{
		return;
	}

	for (unsigned int i = 0; i < MAX_TOTAL_NUMBER_OF_COMPONENTS; i++)
	{
		if (m_IUpdateComponentIds.test(i))
		{
			ECS_ComponentPool* pComponentPool = nullptr;
			short int storedPool = -1;

			PoolComponentMask mask;
			mask.set(i);

			Iterator end = EndIterator(mask);
			for (Iterator it = BeginIterator(mask); it != end; ++it)
			{
				if (it.m_uCurrentPoolId != storedPool)
				{
					// We store the T Pool to directly tell it to update the T.
					storedPool = it.m_uCurrentPoolId;
					pComponentPool = GetEntityPool(storedPool)->GetComponentPool(i);
				}

				pComponentPool->UpdateElement(it.GetCurrentEntityIndex(), _deltaTime);
			}
		}
	}
}

void ECS_PoolManager::RenderEntities()
{
	if (!m_IRenderComponentIds.any())
	{
		return;
	}

	for (unsigned int i = 0; i < MAX_TOTAL_NUMBER_OF_COMPONENTS; i++)
	{
		if (m_IRenderComponentIds.test(i))
		{
			ECS_ComponentPool* pComponentPool = nullptr;
			short int storedPool = -1;

			PoolComponentMask renderComponentsMask;
			ComponentIndex transformIndex = static_cast<unsigned char>(-1); // When we enter a new Pool, we also store which Transform component has been initialized in it (only the first one, Pools shouldn't have multiple types of Transforms).
			renderComponentsMask.set(i);

			Iterator end = EndIterator(renderComponentsMask);
			for (Iterator it = BeginIterator(renderComponentsMask); it != end; ++it)
			{
				if (it.m_uCurrentPoolId != storedPool)
				{
					// We store the T Pool to directly tell it to update the T.
					storedPool = it.m_uCurrentPoolId;
					
					EntityComponentMask validTransformComponentsMask = GetEntityPool(storedPool)->ConvertPoolMaskToEntityMask(m_ITransformComponentIds);
					if (validTransformComponentsMask.none())
					{
						it.AdvanceToEndOfEntityPool(); // We cannot render anything without a Transform.
						continue;
					}

					// Storing the T Index of the Transform T we will be using.
					for (ComponentIndex i = 0; i < MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL; i++)
					{
						if (validTransformComponentsMask.test(i))
						{
							transformIndex = i;
							break;
						}
					}

					pComponentPool = GetEntityPool(storedPool)->GetComponentPool(i);
				}

				if (it.HasComponentEnabled(transformIndex))
				{
					pComponentPool->CallStoredFunctionWithObjectParam(it.GetCurrentEntityIndex(), it.GetComponent(transformIndex));
				}
			}
		}
	}
}

EntityID ECS_PoolManager::FindComponentOwnerEntity(void* _component) const
{
	for (int poolIndex = 0; poolIndex < m_pools.size(); poolIndex++)
	{
		for (int componentPoolIndex = 0; componentPoolIndex < m_pools[poolIndex].GetComponentPoolsCount(); componentPoolIndex++)
		{
			const ECS_ComponentPool* componentPool = m_pools[poolIndex].m_componentPools[componentPoolIndex];
			const int componentIndex = componentPool->CalculateElementIndex(_component);

			if (componentIndex != -1)
			{
				return m_pools[poolIndex].m_entities[componentIndex].m_id;
			}
		}
	}

	return ECS::CONSTANTS::InvalidEntityID();
}

#pragma endregion

#pragma region Pool Management

ECS_EntityPool* ECS_PoolManager::GetEntityPool(PoolID _poolId)
{
	if (HowManyInitializedEPools() < _poolId)
	{
		return nullptr;
	}

	return &(m_pools[_poolId]);
}

#pragma endregion

#pragma region Iterator

ECS_PoolManager::Iterator::Iterator(
	ECS_PoolManager* _pPoolManager, 
	unsigned int _uCurrentPoolId, 
	PoolComponentMask _componentMask, 
	bool _emptyMask)
	: m_pPoolManager{ _pPoolManager }, m_emptyMask{ _emptyMask }, m_uCurrentPoolId{ _uCurrentPoolId }, m_componentMask{ _componentMask }
{
	if (m_uCurrentPoolId < m_pPoolManager->HowManyInitializedEPools() && 
		(m_emptyMask || DoesEntityPoolImplementAllNecessaryComponents(m_uCurrentPoolId)))
	{
		m_currentBeginIterator = m_pPoolManager->GetEntityPool(_uCurrentPoolId)->BeginIterator(m_componentMask, m_emptyMask);
		m_currentActiveIterator = ECS_EntityPool::PoolIterator(m_currentBeginIterator);
		m_currentEndIterator = m_pPoolManager->GetEntityPool(_uCurrentPoolId)->EndIterator(m_componentMask, m_emptyMask);
	}
	else
	{
		m_currentBeginIterator = ECS_EntityPool::PoolIterator::GetEmptyIterator();
		m_currentActiveIterator = ECS_EntityPool::PoolIterator::GetEmptyIterator();
		m_currentEndIterator = ECS_EntityPool::PoolIterator::GetEmptyIterator();
	}
}

void* ECS_PoolManager::Iterator::GetComponent(ComponentIndex _componentIndex)
{
	assert(IsCurrentIteratorValid() && "Trying to Get Component from an invalid Iterator.");

	return m_currentActiveIterator.GetComponent(_componentIndex);
}

ECS_PoolManager::Iterator ECS_PoolManager::BeginIterator(const PoolComponentMask& _mask)
{
	Iterator resultIterator = Iterator(this, 0, _mask, false);

	if (!resultIterator.IsCurrentIteratorValid())
	{
		++resultIterator;
	}

	return resultIterator;
}

#pragma endregion
