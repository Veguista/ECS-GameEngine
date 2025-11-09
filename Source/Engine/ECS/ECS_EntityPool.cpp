#include "ECS_EntityPool.h"
#include "ECS_ComponentPool.h"
#include "ECS_PoolManager.h"
#include <algorithm>

#pragma region Constructors & Destructor

ECS_EntityPool::ECS_EntityPool(ECS_PoolManager* _pPoolManager,
	unsigned int _uNumberOfComponentsInitialized,
	unsigned int _uMaxNumberOfEntities,
	PoolID _poolId)
	: m_pPoolManager{ _pPoolManager },
	m_uMaxNumberOfEntities{ _uMaxNumberOfEntities },
	m_poolId{ _poolId }
{
	assert(_uNumberOfComponentsInitialized < MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL && "A component pool cannot allow more components than the maximum.");
	assert(_uNumberOfComponentsInitialized != 0 && "A component pool cannot be created without allowed Components.");

	// Filling in the T Indexes array with invalid values.
	for (unsigned int i = 0; i < MAX_TOTAL_NUMBER_OF_COMPONENTS; i++)
	{
		m_componentIndexes[i] = ECS::CONSTANTS::InvalidComponentIndex();
	}

	if (_uMaxNumberOfEntities == 0)
	{
		_uMaxNumberOfEntities++;

		assert(false && "WARNING : Cannot create an ECS_EntityPool2 with [0] as maximum of possible Entities. Reseting maximum to 1.\n");
	}

	// Creating the references to our T Pools.
	m_componentPools.resize(MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL, nullptr); // Setting the initial value to nullptr just in case.
};

ECS_EntityPool::~ECS_EntityPool()
{
	auto end = EndIterator();
	for (auto it = BeginIterator(); it != end; ++it)
	{
		RemoveAllComponents(*it);
	}
}

#pragma endregion

#pragma region Entity Management

EntityID ECS_EntityPool::CreateEntity()
{
	assert(m_entities.size() < m_uMaxNumberOfEntities && "Trying to create more Entities than the MAX_NUMBER_OF_ENTITIES for this specific Entity pool.");

	if (!m_freeEntities.empty())
	{
		unsigned int index = m_freeEntities.back();
		m_freeEntities.pop_back();

		return m_entities[index].SetIndex(index); // SetIndex returns the new ID.
	}

	m_entities.push_back(ECS_Entity(ECS::CreateEntityId(m_entities.size(), m_poolId, 0), EntityComponentMask()));
	return m_entities.back().m_id;
}
unsigned int ECS_EntityPool::CreateEntities(unsigned int _numberOfEntitiesToCreate, EntityID* _entitiesIdBuffer)
{
	if (_numberOfEntitiesToCreate == 0)
	{
		return 0;
	}

	unsigned int currentSizeOfEntityVector = m_entities.size();
	unsigned int numberOfFreeEntities = m_freeEntities.size();

	if (m_uMaxNumberOfEntities < (_numberOfEntitiesToCreate + currentSizeOfEntityVector - numberOfFreeEntities))
	{
		assert(false && "Trying to create more Entities than the MAX_NUMBER_OF_ENTITIES for this specific Entity pool.");

		// We correct how many entities we create. This is not ideal, but if handled correctly by the user of the function 
		// (and if asserts are disabled), it should avoid throwing an error.
		_numberOfEntitiesToCreate = m_uMaxNumberOfEntities - currentSizeOfEntityVector + numberOfFreeEntities;
	}

	for (unsigned int i = 0; i < _numberOfEntitiesToCreate; i++)
	{
		if (numberOfFreeEntities == 0)
		{
			m_entities.push_back(ECS_Entity(ECS::CreateEntityId(currentSizeOfEntityVector++, m_poolId, 0), EntityComponentMask()));
			_entitiesIdBuffer[i] = m_entities.back().m_id;
		}
		else
		{
			_entitiesIdBuffer[i] = m_entities[m_freeEntities.back()].SetIndex(m_freeEntities.back());
			m_freeEntities.pop_back();
			numberOfFreeEntities--;
		}
	}

	return _numberOfEntitiesToCreate;
}

bool ECS_EntityPool::IsEntityDeleted(EntityID _id) const
{
	assert(ECS::GetPoolFromId(_id) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

	return m_entities[ECS::GetIndexFromId(_id)].m_id != _id;
}

void ECS_EntityPool::DestroyEntity(EntityID _entityId)
{
	assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

	unsigned int entityIndex = ECS::GetIndexFromId(_entityId);
	unsigned int entityVersion = ECS::GetVersionFromId(m_entities[entityIndex].m_id);

	assert(!IsEntityDeleted(entityIndex) && "Trying to destroy an entity that no longer exist.");

	assert((entityIndex + 1) != ECS::CONSTANTS::InvalidEntityIndex() && "Entity reached maximum number of versions.");

	//// Checking if our Entity had a T with a children architecture that required to also destroy the children.
	//if constexpr (IsFirstTypeContainedInRestOfTypes<C_Transform2D_PlusParenting, ComponentTypes...>())
	//{
	//	if (HasComponentEnabled<C_Transform2D_PlusParenting>(_entityId))
	//	{
	//		C_Transform2D_PlusParenting* transform = GetComponent<C_Transform2D_PlusParenting>(_entityId);

	//		size_t numberOfChildren = transform->NumberOfChildren();

	//		for (size_t childIndex = 0; childIndex < numberOfChildren; childIndex++)
	//		{
	//			unsigned int entityIndex = m_componentPools[GetComponentId<C_Transform2D_PlusParenting>()]->CalculateElementIndex(transform->INTERNAL_GetChildWithoutChecks(childIndex));
	//			DestroyEntity(m_entities[entityIndex].m_id);
	//		}
	//	}
	//}

	// Calling the destructor of all active Components in the Entity.
	RemoveAllComponents(entityIndex);

	m_entities[entityIndex].m_id = ECS::CreateEntityId(ECS::CONSTANTS::InvalidEntityIndex(), m_poolId, entityVersion + 1);

	m_freeEntities.push_back(entityIndex);
}
void ECS_EntityPool::DestroyEntity(unsigned int _entityIndex)
{
	assert(!IsEntityDeleted(_entityIndex) && "Trying to destroy an entity that no longer exist.");

	unsigned int _entityVersion = ECS::GetVersionFromId(m_entities[_entityIndex].m_id);

	assert(_entityVersion + 1 != ECS::CONSTANTS::InvalidEntityVersion() && "Entity reached maximum number of versions.");

	//// Checking if our Entity had a T with a children architecture that required to also destroy the children.
	//if constexpr (IsFirstTypeContainedInRestOfTypes<C_Transform2D_PlusParenting, ComponentTypes...>())
	//{
	//	if (HasComponentEnabled<C_Transform2D_PlusParenting>(_entityId))
	//	{
	//		C_Transform2D_PlusParenting* transform = GetComponent<C_Transform2D_PlusParenting>(_entityId);

	//		size_t numberOfChildren = transform->NumberOfChildren();

	//		for (size_t childIndex = 0; childIndex < numberOfChildren; childIndex++)
	//		{
	//			unsigned int entityIndex = m_componentPools[GetComponentId<C_Transform2D_PlusParenting>()]->CalculateElementIndex(transform->INTERNAL_GetChildWithoutChecks(childIndex));
	//			DestroyEntity(m_entities[entityIndex].m_id);
	//		}
	//	}
	//}


	// Calling the destructor of all active Components in the Entity.
	RemoveAllComponents(_entityIndex);

	m_entities[_entityIndex].m_id = ECS::CreateEntityId(ECS::CONSTANTS::InvalidEntityIndex(), m_poolId, _entityVersion + 1);

	m_freeEntities.push_back(_entityIndex);
}

void ECS_EntityPool::DestroyEntities(EntityID* _entityIdBuffer, unsigned int _lengthOfBuffer)
{
	assert(_entityIdBuffer != nullptr && "Trying to destroy the entities in an array but the pointer to the array was nullptr.");
	assert(_lengthOfBuffer > 0 && "Trying to destroy the entities in an array but the length of the array was 0.");
	assert(ECS::GetPoolFromId(_entityIdBuffer[0]) == m_poolId && "Trying to make Pool interact with Entities that have incorrect Pool IDs.");

	for (unsigned int i = 0; i < _lengthOfBuffer; i++)
	{
		DestroyEntity(_entityIdBuffer[i]);
	}
}
void ECS_EntityPool::DestroyEntities(unsigned int* _entityIndexBuffer, unsigned int _lengthOfBuffer)
{
	assert(_entityIndexBuffer != nullptr && "Trying to destroy the entities in an array but the pointer to the array was nullptr.");
	assert(_lengthOfBuffer > 0 && "Trying to destroy the entities in an array but the length of the array was 0.");

	for (unsigned int i = 0; i < _lengthOfBuffer; i++)
	{
		DestroyEntity(_entityIndexBuffer[i]);
	}
}

void ECS_EntityPool::DestroyAllEntities()
{
	auto end = EndIterator();
	for (auto it = BeginIterator(); it != end; ++it)
	{
		DestroyEntity(*it);
	}
}

#pragma endregion

#pragma region T Management

bool ECS_EntityPool::HasComponentEnabled(EntityID _entityId, unsigned int _componentIndex) const
{
	if (!HasComponentBeenInitialized(_componentIndex))
	{
		assert(false && "Checking whether a component is enabled but the component wasn't initialized in this Entity Pool.");

		return false;
	}

	if (IsEntityDeleted(_entityId))
	{
		assert(false && "Trying to access an entity that no longer exist.");

		return false;
	}

	return m_entities[ECS::GetIndexFromId(_entityId)].m_componentMask.test(_componentIndex);
}
bool ECS_EntityPool::HasComponentEnabled(unsigned int _entityIndex, unsigned int _componentIndex) const
{
	if (!HasComponentBeenInitialized(_componentIndex))
	{
		assert(false && "Checking whether a component is enabled but the component wasn't initialized in this Entity Pool.");

		return false;
	}

	if (IsEntityDeleted(_entityIndex))
	{
		assert(false && "Trying to access an entity that no longer exist.");

		return false;
	}

	return m_entities[_entityIndex].m_componentMask.test(_componentIndex);
}

void* ECS_EntityPool::GetComponent(unsigned int _entityIndex, unsigned int _componentIndex) const
{
	if (!HasComponentBeenInitialized(_componentIndex))
	{
		assert(false && "Trying to Get a component that wasn't initialized in this Entity Pool.");

		return nullptr;
	}

	if (IsEntityDeleted(_entityIndex))
	{
		assert(false && "Trying to Get a component from an entity that no longer exist.");

		return nullptr;
	}

	// If the component is disabled in the Entity, we return nullptr.
	if (!(m_entities[_entityIndex].m_componentMask.test(_componentIndex)))
	{
		return nullptr;
	}

	// Otherwise, we return the pointer to the component of our entity.
	return m_componentPools[_componentIndex]->GetElement(_entityIndex);
}

void* ECS_EntityPool::AssignComponent(unsigned int _entityIndex, unsigned int _componentIndex)
{
	assert(HasComponentBeenInitialized(_componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

	if (IsEntityDeleted(_entityIndex))
	{
		assert(false && "Trying to assign a component to an entity that no longer exist.");

		return nullptr;
	}

	return INTERNAL_AssignComponentWithoutChecks(_entityIndex, _componentIndex);
}

void* ECS_EntityPool::AssignComponentByCopy(unsigned int _entityIndex, unsigned int _componentIndex, const void* _other)
{
	assert(HasComponentBeenInitialized(_componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

	if (IsEntityDeleted(_entityIndex))
	{
		assert(false && "Trying to assign a component to an entity that no longer exist.");

		return nullptr;
	}

	return INTERNAL_AssignComponentByCopyWithoutChecks(_entityIndex, _componentIndex, _other);
}

void ECS_EntityPool::AssignComponentToMultipleEntities(unsigned int* _entityIndexArray, unsigned int _arrayLength, unsigned int _componentIndex)
{
	assert(HasComponentBeenInitialized(_componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");
	assert(_entityIndexArray != nullptr && "Trying to assign a component to multiple entities but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to assign a component to multiple indexes but the number of entities is 0.");

	// Checking if any of the Entities is deleted or already has that component enabled.
	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		if (IsEntityDeleted(_entityIndexArray[i]))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");
			return;
		}

		assert(!HasComponentEnabled(_entityIndexArray[i], _componentIndex) && "Trying to assign a component to an entity that already has that component assigned.");
	
		m_entities[_entityIndexArray[i]].m_componentMask.set(_componentIndex);
	}

	m_componentPools[_componentIndex]->CreateElements(_entityIndexArray, _arrayLength);
}

void ECS_EntityPool::AssignComponentToMultipleEntitiesByCopy(unsigned int* _entityIndexArray, unsigned int _arrayLength, unsigned int _componentIndex, const void* _other)
{
	assert(HasComponentBeenInitialized(_componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");
	assert(_entityIndexArray != nullptr && "Trying to assign a component to multiple entities but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to assign a component to multiple indexes but the number of entities is 0.");

	// Checking if any of the Entities is deleted or already has that component enabled.
	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		if (IsEntityDeleted(_entityIndexArray[i]))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");
			return;
		}

		assert(!HasComponentEnabled(_entityIndexArray[i], _componentIndex) && "Trying to assign a component to an entity that already has that component assigned.");
	
		m_entities[_entityIndexArray[i]].m_componentMask.set(_componentIndex);
	}

	m_componentPools[_componentIndex]->CreateElementsWithCopyConstructor(_entityIndexArray, _arrayLength, _other);
}

int ECS_EntityPool::RemoveComponent(EntityID _entityId, unsigned int _componentIndex)
{
	assert(HasComponentBeenInitialized(_componentIndex) && "Trying to remove a component that wasn't initialized in this Entity Pool from an entity.");
	assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

	if (IsEntityDeleted(_entityId))
	{
		assert(false && "Trying to remove a component from an entity that no longer exist.");

		return -1;
	}

	return INTERNAL_RemoveComponentWithoutChecks(ECS::GetIndexFromId(_entityId), _componentIndex);
}
int ECS_EntityPool::RemoveComponent(unsigned int _entityIndex, unsigned int _componentIndex)
{
	assert(HasComponentBeenInitialized(_componentIndex) && "Trying to remove a component that wasn't initialized in this Entity Pool from an entity.");

	if (IsEntityDeleted(_entityIndex))
	{
		assert(false && "Trying to remove a component from an entity that no longer exist.");

		return -1;
	}

	return INTERNAL_RemoveComponentWithoutChecks(_entityIndex, _componentIndex);
}

int ECS_EntityPool::RemoveAllComponents(EntityID _entityId)
{
	assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

	if (IsEntityDeleted(_entityId))
	{
		assert(false && "Trying to remove all components from an entity that no longer exist.");

		return -1;
	}

	unsigned int entityIndex = ECS::GetIndexFromId(_entityId);
	EntityComponentMask mask = m_entities[entityIndex].m_componentMask;

	for (unsigned int componentIndex = 0; componentIndex < MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL; componentIndex++)
	{
		if (mask.test(componentIndex))
		{
			if (m_componentPools[componentIndex]->m_delayedDeleterFunct != nullptr)
			{
				m_componentPools[componentIndex]->CallElementDestructor(entityIndex);
			}

			m_entities[entityIndex].m_componentMask.reset(componentIndex);
		}
	}

	return 0;
}
int ECS_EntityPool::RemoveAllComponents(unsigned int _entityIndex)
{
	if (IsEntityDeleted(_entityIndex))
	{
		assert(false && "Trying to remove all components from an entity that no longer exist.");

		return -1;
	}

	EntityComponentMask mask = m_entities[_entityIndex].m_componentMask;

	for (unsigned int componentIndex = 0; componentIndex < MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL; componentIndex++)
	{
		if (mask.test(componentIndex))
		{
			if (m_componentPools[componentIndex]->m_delayedDeleterFunct != nullptr)
			{
				m_componentPools[componentIndex]->CallElementDestructor(_entityIndex);
			}
			m_entities[_entityIndex].m_componentMask.reset(componentIndex);
		}
	}

	return 0;
}

/// <summary>
/// Converts a PoolComponentMask into an EntityComponentMask valid for this specific pool.
/// </summary>
/// <param name="_poolMask">The Pool T Mask to be converted. Must not contain Components that have not been initialized in this Entity Pool.</param>
/// <returns></returns>
EntityComponentMask ECS_EntityPool::ConvertPoolMaskToEntityMask(const PoolComponentMask& _poolMask) const
{
	EntityComponentMask resultMask;

	for (unsigned int i = 0; i < MAX_TOTAL_NUMBER_OF_COMPONENTS; i++)
	{
		if (_poolMask.test(i))
		{
			assert(GetComponentIndex(i) != ECS::CONSTANTS::InvalidComponentIndex() && "Converting a Pool Component Mask into an Entity Component Mask but the Pool Mask contained Component Ids that have not been initialized in this Entity Pool.");
			resultMask.set(GetComponentIndex(i));
		}
	}

	return resultMask;
}

#pragma endregion

#pragma region INTERNAL

void* ECS_EntityPool::INTERNAL_AssignComponentWithoutChecks(unsigned int _entityIndex, unsigned int _componentIndex)
{
	// We check if this component might already be assigned to this EntityID.
	if (INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, _componentIndex))
	{
		assert(false && "Trying to assign a component to an entity that already has that component enabled.");

		return GetComponent(_entityIndex, _componentIndex);
	}

	m_entities[_entityIndex].m_componentMask.set(_componentIndex);

	// This creates an object of Type T at the location provided by the T Pool.
	return m_componentPools[_componentIndex]->CreateElement(_entityIndex);
}
void* ECS_EntityPool::INTERNAL_AssignComponentByCopyWithoutChecks(unsigned int _entityIndex, unsigned int _componentIndex, const void* _elementToCopy)
{
	// We check if this component might already be assigned to this EntityID.
	if (INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, _componentIndex))
	{
		assert(false && "Trying to assign a component to an entity that already has that component enabled.");

		return GetComponent(_entityIndex, _componentIndex);
	}

	m_entities[_entityIndex].m_componentMask.set(_componentIndex);

	// This creates an object of Type T at the location provided by the T Pool.
	return m_componentPools[_componentIndex]->CreateElementWithCopyConstructor(_entityIndex, _elementToCopy);
}

int ECS_EntityPool::INTERNAL_RemoveComponentWithoutChecks(unsigned int _entityIndex, unsigned int _componentIndex)
{
	// We check if this Entity might not have this component enabled.
	if (!INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, _componentIndex))
	{
		assert(false && "Trying to remove a component from an entity that didn't have it enabled.");

		return -2;
	}

	if (m_componentPools[_componentIndex]->m_delayedDeleterFunct != nullptr)
	{
		m_componentPools[_componentIndex]->CallElementDestructor(_entityIndex);
	}
	m_entities[_entityIndex].m_componentMask.reset(_componentIndex);

	return 0;
}

#pragma endregion

#pragma region Iterator

EntityID ECS_EntityPool::PoolIterator::operator*() const
{
	return m_pEntityPool->m_entities[m_uCurrentEntityIndex].m_id;
}
bool ECS_EntityPool::PoolIterator::operator==(const PoolIterator& other) const
{																													 // The last part here checks if we are at the end of the iterator, which behaves differently when compared.
	return m_uCurrentEntityIndex == other.m_uCurrentEntityIndex || m_uCurrentEntityIndex == m_pEntityPool->m_entities.size();
}
bool ECS_EntityPool::PoolIterator::operator!=(const PoolIterator& other) const
{
	return m_uCurrentEntityIndex != other.m_uCurrentEntityIndex && m_uCurrentEntityIndex != m_pEntityPool->m_entities.size();
}

ECS_EntityPool::PoolIterator& ECS_EntityPool::PoolIterator::operator++()
{
	do
	{
		m_uCurrentEntityIndex++;
	}
	while (m_uCurrentEntityIndex < m_pEntityPool->m_entities.size() && !IsCurrentIndexValidForIterator());

	return *this;
}
ECS_EntityPool::PoolIterator& ECS_EntityPool::PoolIterator::operator--()
{
	do
	{
		m_uCurrentEntityIndex--;
	}
	while (m_uCurrentEntityIndex != ECS::CONSTANTS::InvalidEntityIndex() && !IsCurrentIndexValidForIterator());

	return *this;
}

bool ECS_EntityPool::PoolIterator::IsCurrentIndexValidForIterator() const
{
	if (!m_pEntityPool->IsEntityDeleted(m_uCurrentEntityIndex))
	{
		if (m_emptyMask)
		{
			return true;
		}
		else
		{
			return m_componentMask == (m_componentMask & m_pEntityPool->m_entities[m_uCurrentEntityIndex].m_componentMask);
		}
	}

	return false;
}

ECS_EntityPool::PoolIterator ECS_EntityPool::BeginIterator(const PoolComponentMask& _poolMask, bool _emptyMask)
{
	unsigned int firstIndex = 0;
	EntityComponentMask mask;
	if (_emptyMask)
	{
		while (firstIndex < m_entities.size() && IsEntityDeleted(firstIndex))
		{
			firstIndex++;
		}
	}
	else
	{
		mask = ConvertPoolMaskToEntityMask(_poolMask);

		while (firstIndex < m_entities.size() &&
			(mask != (mask & m_entities[firstIndex].m_componentMask) || IsEntityDeleted(firstIndex))
			)
		{
			firstIndex++;
		}
	}

	return PoolIterator(this, firstIndex, mask, _emptyMask);
}

ECS_EntityPool::PoolIterator ECS_EntityPool::EndIterator(const PoolComponentMask& _poolMask, bool _emptyMask)
{
	{
		unsigned int lastIndex = m_entities.size();

		EntityComponentMask mask;

		if (!_emptyMask)
		{
			mask = ConvertPoolMaskToEntityMask(_poolMask);
		}

		return PoolIterator(this, lastIndex, mask, _emptyMask);
	}
}

#pragma endregion

