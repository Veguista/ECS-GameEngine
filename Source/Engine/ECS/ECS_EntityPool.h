#pragma once

#include "ECS_Configuration.h"
#include "ECS_Typedefs.h"
#include "ECS_Constants.h"
#include "ECS_SupportingFunctions.h"
#include "ECS_ComponentPool.h"
#include "ECS_Entity.h"
#include "assert.h"
#include <array>
#include <vector>

class ECS_PoolManager;

class ECS_EntityPool
{
	std::array<ComponentIndex, MAX_TOTAL_NUMBER_OF_COMPONENTS> m_componentIndexes{};

	unsigned int m_uNumberOfInitializedComponents{ 0 };
	const unsigned int m_uMaxNumberOfEntities;
	const PoolID m_poolId;
	const ECS_PoolManager* m_pPoolManager;

	std::vector<unsigned int> m_freeEntities;

public:
	std::vector<ECS_Entity> m_entities;
	std::vector<ECS_ComponentPool*> m_componentPools;

#pragma region Constructors & Destructor

private:
	ECS_EntityPool(ECS_PoolManager* _pPoolManager,
		unsigned int _uNumberOfComponentsInitialized,
		unsigned int _uMaxNumberOfEntities,
		PoolID _poolId);

public:
	/// <summary>
	/// INTERNAL use only. Use the function CreateEntityPoolAtLocation for general use instead.
	/// </summary>
	ECS_EntityPool()
		: m_uMaxNumberOfEntities{ ECS::CONSTANTS::InvalidEntityIndex() },
		m_poolId{ ECS::CONSTANTS::InvalidPoolId() },
		m_pPoolManager{ nullptr } {};

	template<typename... ComponentTypes>
	static int CreateEntityPoolAtLocation(
		void* _location, 
		ECS_PoolManager* _pPoolManager, 
		unsigned int _uMaxNumberOfEntities, 
		PoolID _poolId)
	{
		if constexpr(sizeof...(ComponentTypes) > 0)
		{
			ECS_EntityPool* myEntityPool =
				new (_location) ECS_EntityPool(_pPoolManager, sizeof...(ComponentTypes), _uMaxNumberOfEntities, _poolId);
			
			myEntityPool->INTERNAL_InitializeComponents<ComponentTypes...>();

			// Forcing the creation of at least one Entity. 
			// This avoids memory errors when checking the std::vector<ECS_Entity> from outside the class.
			myEntityPool->DestroyEntity(myEntityPool->CreateEntity());

			return 0;
		}
		else
		{
			assert(false && "Cannot create an Entity Pool without valid Components.");
			return -1;
		}
	}

	 ~ECS_EntityPool();

#pragma endregion

#pragma region General Methods
public:
	inline PoolID GetPoolId() const { return m_poolId; };

	inline ECS_ComponentPool* GetComponentPool(const unsigned int _componentId) const 
		{ return m_componentPools[GetComponentIndex(_componentId)]; };

	inline int GetComponentPoolsCount() const { return m_uNumberOfInitializedComponents; };

#pragma endregion

#pragma region Entity Management

	EntityID CreateEntity();

	/// <summary>
	/// Tries to create a set number of entities at the Entity pool.
	/// It also stores the newly created EntityIDs in a given EntityID-buffer.
	/// </summary>
	/// <param name="_numberOfEntitiesToCreate">: How many entities we want to create.</param>
	/// <param name="_entitiesIdBuffer">: Buffer in which to store the newly generated EntityIDs. 
	/// Must be of size similar or bigger than the number of Entities we are trying to generate.</param>
	/// <returns>The number of Entities that were successfully created.</returns>
	unsigned int CreateEntities(unsigned int _numberOfEntitiesToCreate, EntityID* _entitiesIdBuffer);

	template<typename... Components>
	EntityID CreateEntityWithComponents()
	{
		assert(m_entities.size() < m_uMaxNumberOfEntities && "Trying to create more Entities than the MAX_NUMBER_OF_ENTITIES for this specific Entity pool.");

		EntityID id = CreateEntity();

		AssignComponents<Components...>(id);

		return id;
	}
	
	bool IsEntityDeleted(EntityID _id) const;
	inline bool IsEntityDeleted(const unsigned int _entityIndex) const 
		{ return ECS::GetIndexFromId(m_entities[_entityIndex].m_id) == ECS::CONSTANTS::InvalidEntityIndex(); }

	void DestroyEntity(EntityID _entityId);
	void DestroyEntity(unsigned int _entityIndex);

	void DestroyEntities(EntityID* _entityIdBuffer, unsigned int _lengthOfBuffer);
	void DestroyEntities(unsigned int* _entityIndexBuffer, unsigned int _lengthOfBuffer);

	void DestroyAllEntities();

#pragma endregion

#pragma region Component Management
public:

	template<typename T>
	ComponentIndex GetComponentIndex() const
	{
		assert(HasComponentBeenInitialized<T>() && "Trying to obtain a componentID inside a pool that wasn't created with set TYPE.");

		return m_componentIndexes[ECS::GetComponentId<T>()];
	}
	inline ComponentIndex GetComponentIndex(const unsigned int _componentId) const 
		{	return m_componentIndexes[_componentId]; };

	template<typename T>
	bool HasComponentBeenInitialized() const
	{
		return m_componentIndexes[ECS::GetComponentId<T>()] != ECS::CONSTANTS::InvalidComponentIndex();
	}
	template<typename FirstComponent, typename... ComponentTypes>
	bool HaveComponentsBeenInitialized() const
	{
		if constexpr (sizeof...(ComponentTypes) == 0)
		{
			return HasComponentBeenInitialized<FirstComponent>();
		}
		else
		{
			return HasComponentBeenInitialized<FirstComponent>() && HaveComponentsBeenInitialized<ComponentTypes...>();
		}
	}
	inline bool HasComponentBeenInitialized(const unsigned int _componentIndex) const 
		{	return _componentIndex != ECS::CONSTANTS::InvalidComponentIndex(); };

	template<typename T>
	bool HasComponentEnabled(EntityID _entityId) const
	{
		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

		unsigned int componentIndex = GetComponentIndex<T>();

		if (!HasComponentBeenInitialized(componentIndex))
		{
			assert(false && "Checking whether a component is enabled but the component wasn't initialized in this Entity Pool.");

			return false;
		}

		if (IsEntityDeleted(_entityId))
		{
			assert(false && "Trying to access an entity that no longer exist.");

			return false;
		}

		return m_entities[ECS::GetIndexFromId(_entityId)].m_componentMask.test(componentIndex);
	}
	template<typename T>
	bool HasComponentEnabled(unsigned int _entityIndex) const
	{
		unsigned int componentIndex = GetComponentIndex<T>();

		if (!HasComponentBeenInitialized(componentIndex))
		{
			assert(false && "Checking whether a component is enabled but the component wasn't initialized in this Entity Pool.");

			return false;
		}

		if (IsEntityDeleted(_entityIndex))
		{
			assert(false && "Trying to access an entity that no longer exist.");

			return false;
		}

		return m_entities[_entityIndex].m_componentMask.test(componentIndex);
	}
	bool HasComponentEnabled(EntityID _entityId, unsigned int _componentIndex) const;
	bool HasComponentEnabled(unsigned int _entityIndex, unsigned int _componentIndex) const;

	template<typename T>
	T* GetComponent(EntityID _entityId)
	{
		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

		const unsigned int uComponentIndex = GetComponentIndex<T>();
		const unsigned int entityIndex = ECS::GetIndexFromId(_entityId);

		if (!HasComponentBeenInitialized(uComponentIndex))
		{
			assert(false && "Trying to Get a component that wasn't initialized in this Entity Pool.");

			return nullptr;
		}

		if (IsEntityDeleted(_entityId))
		{
			assert(false && "Trying to Get a component from an entity that no longer exist.");

			return nullptr;
		}

		// If the component is disabled in the Entity, we return nullptr.
		if (!(m_entities[entityIndex].m_componentMask.test(uComponentIndex)))
		{
			return nullptr;
		}

		// Otherwise, we return the pointer to the component of our entity.
		return reinterpret_cast<T*>(m_componentPools[uComponentIndex]->GetElement(entityIndex));
	}
	template<typename T>
	T* GetComponent(unsigned int _entityIndex)
	{
		const unsigned int uComponentIndex = GetComponentIndex<T>();

		if (!HasComponentBeenInitialized(uComponentIndex))
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
		if (!(m_entities[_entityIndex].m_componentMask.test(uComponentIndex)))
		{
			return nullptr;
		}

		// Otherwise, we return the pointer to the component of our entity.
		return reinterpret_cast<T*>(m_componentPools[uComponentIndex]->GetElement(_entityIndex));
	}
	void* GetComponent(unsigned int _entityIndex, unsigned int _componentIndex) const;

	template<typename T>
	T* AssignComponent(EntityID _entityId)
	{
		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");
		assert(HasComponentBeenInitialized<T>() && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

		if (IsEntityDeleted(_entityId))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");

			return nullptr;
		}

		return INTERNAL_AssignComponentWithoutChecks<T>(_entityId);
	}
	template<typename T>
	T* AssignComponent(unsigned int _entityIndex)
	{
		assert(HasComponentBeenInitialized<T>() && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

		if (IsEntityDeleted(_entityIndex))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");

			return nullptr;
		}

		return INTERNAL_AssignComponentWithoutChecks<T>(_entityIndex);
	}
	void* AssignComponent(unsigned int _entityIndex, unsigned int _componentIndex);
	template<typename T, typename... ConstructorValues>
	T* AssignComponent(EntityID _entityId, ConstructorValues... args)
	{
		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");
		assert(HasComponentBeenInitialized<T>() && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

		unsigned int entityIndex = ECS::GetIndexFromId(_entityId);
		const unsigned int componentIndex = GetComponentIndex<T>();

		if (IsEntityDeleted(entityIndex))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");

			return nullptr;
		}
		if (INTERNAL_IsComponentEnabledWithoutChecks(entityIndex, componentIndex))
		{
			assert(false && "Trying to assign a component to an entity that already has that Component enabled.");

			return reinterpret_cast<T*>(INTERNAL_GetComponentWithoutChecks(entityIndex, componentIndex));
		}

		m_entities[entityIndex].m_componentMask.set(componentIndex);

		// I believe this is creating an object of Type T at the location provided by the T Pool
		return new (m_componentPools[componentIndex]->GetElement(entityIndex)) T(args...);
	}
	template<typename T, typename... ConstructorValues>
	T* AssignComponent(unsigned int _entityIndex, ConstructorValues... args)
	{
		assert(HasComponentBeenInitialized<T>() && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

		const unsigned int componentIndex = GetComponentIndex<T>();

		if (IsEntityDeleted(_entityIndex))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");

			return nullptr;
		}
		if (INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, componentIndex))
		{
			assert(false && "Trying to assign a component to an entity that already has that Component enabled.");

			return reinterpret_cast<T*>(INTERNAL_GetComponentWithoutChecks(_entityIndex, componentIndex));
		}

		m_entities[_entityIndex].m_componentMask.set(componentIndex);

		// I believe this is creating an object of Type T at the location provided by the T Pool
		return new (m_componentPools[componentIndex]->GetElement(_entityIndex)) T(args...);
	}

	template<typename T>
	T* AssignComponentByCopy(EntityID _entityId, T& _other)
	{
		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");
		assert(HasComponentBeenInitialized<T>() && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

		if (IsEntityDeleted(_entityId))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");

			return nullptr;
		}

		return INTERNAL_AssignComponentByCopyWithoutChecks<T>(_entityId, _other);
	}
	template<typename T>
	T* AssignComponentByCopy(unsigned int _entityIndex, T& _other)
	{
		assert(HasComponentBeenInitialized<T>() && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");

		if (IsEntityDeleted(_entityIndex))
		{
			assert(false && "Trying to assign a component to an entity that no longer exist.");

			return nullptr;
		}

		return INTERNAL_AssignComponentByCopyWithoutChecks<T>(_entityIndex, _other);
	}
	void* AssignComponentByCopy(unsigned int _entityIndex, unsigned int _componentIndex, const void* _other);

	template<typename T, typename... ComponentTypes>
	int AssignComponents(EntityID _entityId)
	{
		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");

		if (IsEntityDeleted(_entityId))
		{
			assert(false && "Trying to assign components to an entity that no longer exist.");

			return -1;
		}

		if constexpr (sizeof...(ComponentTypes) > 0)
		{
			assert(HasComponentBeenInitialized<T>() && HaveComponentsBeenInitialized<ComponentTypes...>() && "Trying to assign components that weren't initialized in this Entity Pool to an entity.");

			INTERNAL_AssignMultipleComponentsWithoutChecks<T, ComponentTypes...>(_entityId);
		}
		else
		{
			INTERNAL_AssignComponentWithoutChecks<T>(_entityId);
		}

		return 0;
	}

	template<typename T>
	void AssignComponentToMultipleEntities(EntityID* _entityIdArray, unsigned int _arrayLength)
	{
		unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized(componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");
		assert(_entityIdArray != nullptr && "Trying to assign a component to multiple entities but the entity array is nullptr.");
		assert(_arrayLength > 0 && "Trying to assign a component to multiple entities but the number of entities is 0.");
		assert(ECS::GetPoolFromId(_entityIdArray[0]) == m_poolId && "Trying to make Pool interact with Entities that have incorrect Pool IDs.");

		for (unsigned int i = 0; i < _arrayLength; i++)
		{
			if (IsEntityDeleted(_entityIdArray[i]))
			{
				assert(false && "Trying to assign a component to an entity that no longer exist.");
				return;
			}

			assert(!HasComponentEnabled(ECS::GetIndexFromId(_entityIdArray[i]), componentIndex) && "Trying to assign a component to an entity that already has that component assigned.");

			m_entities[ECS::GetIndexFromId(_entityIdArray[i])].m_componentMask.set(componentIndex);
		}

		m_componentPools[componentIndex]->CreateElements(_entityIdArray, _arrayLength);
	}
	template<typename T>
	void AssignComponentToMultipleEntities(unsigned int* _entityIndexArray, unsigned int _arrayLength)
	{
		unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized(componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to an entity.");
		assert(_entityIndexArray != nullptr && "Trying to assign a component to multiple entities but the entity array is nullptr.");
		assert(_arrayLength > 0 && "Trying to assign a component to multiple entities but the number of entities is 0.");

		for (unsigned int i = 0; i < _arrayLength; i++)
		{
			if (IsEntityDeleted(_entityIndexArray[i]))
			{
				assert(false && "Trying to assign a component to an entity that no longer exist.");
				return;
			}

			assert(!HasComponentEnabled(_entityIndexArray[i], componentIndex) && "Trying to assign a component to an entity that already has that component assigned.");

			m_entities[_entityIndexArray[i]].m_componentMask.set(componentIndex);
		}

		m_componentPools[componentIndex]->CreateElements(_entityIndexArray, _arrayLength);
	}
	template<typename T, typename... ConstructorValues>
	void AssignComponentToMultipleEntities(EntityID* _entityIdArray, unsigned int _arrayLength, ConstructorValues... args)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized(componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to multiple entities.");
		assert(_entityIdArray != nullptr && "Trying to assign a component to multiple entities but the entity array is nullptr.");
		assert(_arrayLength > 0 && "Trying to assign a component to multiple entities but the number of entities is 0.");
		assert(ECS::GetPoolFromId(_entityIdArray[0]) == m_poolId && "Trying to make Pool interact with Entities that have incorrect Pool IDs.");

		for (unsigned int i = 0; i < _arrayLength; i++)
		{
			if (IsEntityDeleted(_entityIdArray[i]))
			{
				assert(false && "Trying to assign a component to an entity that no longer exist.");
				return;
			}

			unsigned int entityIndex = ECS::GetIndexFromId(_entityIdArray[i]);

			m_entities[entityIndex].m_componentMask.set(componentIndex);

			// This is creating an object of Type T at the location provided by the T Pool
			new (m_componentPools[componentIndex]->GetElement(entityIndex)) T(args...);
		}
	}
	template<typename T, typename... ConstructorValues>
	void AssignComponentToMultipleEntities(unsigned int* _entityIndexArray, unsigned int _arrayLength, ConstructorValues... args)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized(componentIndex) && "Trying to assign a component that wasn't initialized in this Entity Pool to multiple entities.");
		assert(_entityIndexArray != nullptr && "Trying to assign a component to multiple entities but the entity array is nullptr.");
		assert(_arrayLength > 0 && "Trying to assign a component to multiple entities but the number of entities is 0.");

		for (unsigned int i = 0; i < _arrayLength; i++)
		{
			if (IsEntityDeleted(_entityIndexArray[i]))
			{
				assert(false && "Trying to assign a component to an entity that no longer exist.");
				return;
			}

			m_entities[_entityIndexArray[i]].m_componentMask.set(componentIndex);

			// This is creating an object of Type T at the location provided by the T Pool
			new (m_componentPools[componentIndex]->GetElement(_entityIndexArray[i])) T(args...);
		}
	}
	void AssignComponentToMultipleEntities(unsigned int* _entityIndexArray, unsigned int _arrayLength, unsigned int _componentIndex);

	template<typename T>
	void AssignComponentToMultipleEntitiesByCopy(EntityID* _entityIdArray, unsigned int _arrayLength, T& _other)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized(componentIndex) && "Trying to copy construct a component that wasn't initialized in this Entity Pool to an entity.");
		assert(_entityIdArray != nullptr && "Trying to copy construct a component to multiple entities but the entity array is nullptr.");
		assert(_arrayLength > 0 && "Trying to copy construct a component to multiple entities but the number of entities is 0.");
		assert(ECS::GetPoolFromId(_entityIdArray[0]) == m_poolId && "Trying to make Pool interact with Entities that have incorrect Pool IDs.");

		for (unsigned int i = 0; i < _arrayLength; i++)
		{
			if (IsEntityDeleted(_entityIdArray[i]))
			{
				assert(false && "Trying to assign a component to an entity that no longer exist.");
				return;
			}

			unsigned int entityIndex = ECS::GetIndexFromId(_entityIdArray[i]);

			assert(!HasComponentEnabled(entityIndex, componentIndex) && "Trying to copy construct a component to an entity that already has that component assigned.");

			m_entities[entityIndex].m_componentMask.set(componentIndex);

			// This is creating an object of Type T at the location provided by the T Pool
			new (m_componentPools[componentIndex]->GetElement(entityIndex)) T(_other);
		}
	}
	template<typename T>
	void AssignComponentToMultipleEntitiesByCopy(unsigned int* _entityIndexArray, unsigned int _arrayLength, T& _other)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized(componentIndex) && "Trying to copy construct a component that wasn't initialized in this Entity Pool to an entity.");
		assert(_entityIndexArray != nullptr && "Trying to copy construct a component to multiple entities but the entity array is nullptr.");
		assert(_arrayLength > 0 && "Trying to copy construct a component to multiple entities but the number of entities is 0.");

		for (unsigned int i = 0; i < _arrayLength; i++)
		{
			if (IsEntityDeleted(_entityIndexArray[i]))
			{
				assert(false && "Trying to assign a component to an entity that no longer exist.");
				return;
			}

			assert(!HasComponentEnabled(_entityIndexArray[i], componentIndex) && "Trying to copy construct a component to an entity that already has that component assigned.");

			m_entities[_entityIndexArray[i]].m_componentMask.set(componentIndex);

			// This is creating an object of Type T at the location provided by the T Pool
			new (m_componentPools[componentIndex]->GetElement(_entityIndexArray[i])) T(_other);
		}
	}
	void AssignComponentToMultipleEntitiesByCopy(unsigned int* _entityIndexArray, unsigned int _arrayLength, unsigned int _componentIndex, const void* _other);

	template<typename T>
	int RemoveComponent(EntityID _entityId)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		assert(ECS::GetPoolFromId(_entityId) == m_poolId && "Trying to make Pool interact with an Entity that has an incorrect Pool ID.");
		assert(HasComponentBeenInitialized<T>() && "Trying to remove a component that wasn't initialized in this Entity Pool from an entity.");

		if (IsEntityDeleted(_entityId))
		{
			assert(false && "Trying to remove a component from an entity that no longer exist.");

			return -1;
		}

		return INTERNAL_RemoveComponentsWithoutChecks<T>(_entityId);
	}
	template<typename T>
	int RemoveComponent(unsigned int _entityIndex)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		assert(HasComponentBeenInitialized<T>() && "Trying to remove a component that wasn't initialized in this Entity Pool from an entity.");

		if (IsEntityDeleted(_entityIndex))
		{
			assert(false && "Trying to remove a component from an entity that no longer exist.");

			return -1;
		}

		return INTERNAL_RemoveComponentsWithoutChecks<T>(_entityIndex);
	}
	int RemoveComponent(EntityID _entityId, unsigned int _componentIndex);
	int RemoveComponent(unsigned int _entityIndex, unsigned int _componentIndex);

	int RemoveAllComponents(EntityID _entityId);
	int RemoveAllComponents(unsigned int _entityIndex);

	template<typename...Types>
	EntityComponentMask ConvertPoolMaskToEntityMask() const
	{
		EntityComponentMask mask;

		if constexpr (sizeof...(Types) >= 1)
		{
			assert(HaveComponentsBeenInitialized<Types...>() && "Trying to create a ComponentMasks with components that weren't initialized in this Entity Pool to an entity.");

			unsigned int componentIds[]{ 0, GetComponentIndex<Types>()... };
			for (unsigned int i = 1; i < sizeof...(Types) + 1; i++)
			{
				mask.set(componentIds[i]);
			}
		}

		return mask;
	}
	EntityComponentMask ConvertPoolMaskToEntityMask(const PoolComponentMask& _poolMask) const;

#pragma endregion

#pragma region INTERNAL

private:
	template <typename FirstComponent, typename... ComponentTypes>
	void INTERNAL_InitializeComponents()
	{
		assert(!HasComponentBeenInitialized<FirstComponent>() && "ERROR : A component cannot be initialized more than once in an ECS_EntityPool.\n");

		// Recording the T Index.
		m_componentIndexes[ECS::GetComponentId<FirstComponent>()] = m_uNumberOfInitializedComponents;
		m_uNumberOfInitializedComponents++;

		// Recording the delayed functions for this component.
		{
			delayed_updater_func delayedUpdaterFunct{ nullptr };
			if constexpr (ECS_INTERNAL::Implements_IECS_Update<FirstComponent>())
			{
				delayedUpdaterFunct = &ECS_INTERNAL::DelayedUpdater<FirstComponent>;
			}

			delayed_constructor_func delayedConstructorFunct{ nullptr };
			if constexpr (ECS_INTERNAL::Implements_ECS_Constructor<FirstComponent>())
			{
				delayedConstructorFunct = &ECS_INTERNAL::DelayedCallConstructorAtLocation<FirstComponent>;
			}

			delayed_destructor_func delayedDeleterFunct{ nullptr };
			if constexpr (ECS_INTERNAL::Implements_ECS_Destructor<FirstComponent>())
			{
				delayedDeleterFunct = &ECS_INTERNAL::DelayedDestructor<FirstComponent>;
			}

			delayed_copy_constructor_func delayedCopyConstructorFunct{ nullptr };
			if constexpr (ECS_INTERNAL::Implements_IECS_CopyConstructor<FirstComponent>())
			{
				delayedCopyConstructorFunct = &ECS_INTERNAL::DelayedCallCopyConstructorAtLocation<FirstComponent>;
			}

			delayed_funct_plus_one_object_param delayedFunctWithOneObjectParam{ nullptr };
			if constexpr (ECS_INTERNAL::Implements_ECS_FunctionWithOneObjectParam<FirstComponent>())
			{
				delayedFunctWithOneObjectParam = &ECS_INTERNAL::DelayedFunctionWithOneObjectParam<FirstComponent>;
			}

			delayed_funct_serialize delayedFunctSerialize{ nullptr };
			delayed_funct_serialize delayedFunctLoad{ nullptr };
			if constexpr (ECS_INTERNAL::Implements_ECS_Serialization<FirstComponent>())
			{
				delayedFunctSerialize = &ECS_INTERNAL::DelayedFunctionSerialize<FirstComponent>;
				delayedFunctLoad = &ECS_INTERNAL::DelayedFunctionLoad<FirstComponent>;
			}

			m_componentPools[GetComponentIndex<FirstComponent>()] = new ECS_ComponentPool(sizeof(FirstComponent), m_uMaxNumberOfEntities,
				delayedUpdaterFunct, delayedConstructorFunct, delayedDeleterFunct, delayedCopyConstructorFunct, delayedFunctWithOneObjectParam, delayedFunctSerialize, delayedFunctLoad);
		}

		// Initializing the next T.
		if constexpr (sizeof...(ComponentTypes) > 0)
		{
			INTERNAL_InitializeComponents<ComponentTypes...>();
		}
	}

	template<typename T>
	inline bool INTERNAL_IsComponentEnabledWithoutChecks(const EntityID _entityId) const
	{
		return m_entities[ECS::GetIndexFromId(_entityId)].m_componentMask.test(GetComponentIndex<T>());
	}
	template<typename T>
	inline bool INTERNAL_IsComponentEnabledWithoutChecks(const unsigned int _entityIndex) const
	{
		return m_entities[_entityIndex].m_componentMask.test(GetComponentIndex<T>());
	}
	inline bool INTERNAL_IsComponentEnabledWithoutChecks(const unsigned int _entityIndex, const unsigned int _componentIndex) const
		{	return m_entities[_entityIndex].m_componentMask.test(_componentIndex); };

	template<typename T>
	T* INTERNAL_AssignComponentWithoutChecks(EntityID _entityId)
	{
		unsigned int entityIndex = ECS::GetIndexFromId(_entityId);
		const unsigned int componentIndex = GetComponentIndex<T>();

		// We check if this component might already be assigned to this EntityID.
		if (INTERNAL_IsComponentEnabledWithoutChecks(entityIndex, componentIndex))
		{
			assert(false && "Trying to assign a component to an entity that already has that component enabled.");

			return INTERNAL_GetComponentWithoutChecks<T>(_entityId);
		}

		m_entities[entityIndex].m_componentMask.set(componentIndex);

		// This creates an object of Type T at the location provided by the T Pool.
		return new (m_componentPools[componentIndex]->GetElement(entityIndex)) T();
	}
	template<typename T>
	T* INTERNAL_AssignComponentWithoutChecks(unsigned int _entityIndex)
	{
		const unsigned int componentIndex = GetComponentIndex<T>();

		// We check if this component might already be assigned to this EntityID.
		if (INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, componentIndex))
		{
			assert(false && "Trying to assign a component to an entity that already has that component enabled.");

			return INTERNAL_GetComponentWithoutChecks<T>(_entityIndex);
		}

		m_entities[_entityIndex].m_componentMask.set(componentIndex);

		// This creates an object of Type T at the location provided by the T Pool.
		return new (m_componentPools[componentIndex]->GetElement(_entityIndex)) T();
	}
	void* INTERNAL_AssignComponentWithoutChecks(unsigned int _entityIndex, unsigned int _componentIndex);

	template<typename T>
	T* INTERNAL_AssignComponentByCopyWithoutChecks(EntityID _entityId, const T& _otherComponent)
	{
		unsigned int entityIndex = ECS::GetIndexFromId(_entityId);
		unsigned int componentIndex = GetComponentIndex<T>();

		// We check if this component might already be assigned to this EntityID.
		if (INTERNAL_IsComponentEnabledWithoutChecks(entityIndex, componentIndex))
		{
			assert(false && "Trying to assign a component to an entity that already has that component enabled.");

			return reinterpret_cast<T*>(GetComponent(entityIndex, componentIndex));
		}

		m_entities[entityIndex].m_componentMask.set(componentIndex);

		// This creates an object of Type T at the location provided by the T Pool.
		return new (m_componentPools[componentIndex]->GetElement(entityIndex)) T(_otherComponent);
	}
	template<typename T>
	T* INTERNAL_AssignComponentByCopyWithoutChecks(unsigned int _entityIndex, const T& _otherComponent)
	{
		unsigned int componentIndex = GetComponentIndex<T>();

		// We check if this component might already be assigned to this EntityID.
		if (INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, componentIndex))
		{
			assert(false && "Trying to assign a component to an entity that already has that component enabled.");

			return reinterpret_cast<T*>(GetComponent(_entityIndex, componentIndex));
		}

		m_entities[_entityIndex].m_componentMask.set(componentIndex);

		// This creates an object of Type T at the location provided by the T Pool.
		return new (m_componentPools[componentIndex]->GetElement(_entityIndex)) T(_otherComponent);
	}
	void* INTERNAL_AssignComponentByCopyWithoutChecks(unsigned int _entityIndex, unsigned int _componentIndex, const void* _elementToCopy);

	template<typename T, typename... ComponentTypes>
	void INTERNAL_AssignMultipleComponentsWithoutChecks(EntityID _entityId)
	{
		unsigned int entityIndex = ECS::GetIndexFromId(_entityId);

		INTERNAL_AssignComponentWithoutChecks(entityIndex, GetComponentIndex<T>());

		if constexpr (sizeof...(ComponentTypes) > 0)
		{
			INTERNAL_AssignMultipleComponentsWithoutChecks<ComponentTypes...>(entityIndex);
		}
	}
	template<typename T, typename... ComponentTypes>
	void INTERNAL_AssignMultipleComponentsWithoutChecks(unsigned int _entityIndex)
	{
		INTERNAL_AssignComponentWithoutChecks(_entityIndex, GetComponentIndex<T>());

		if constexpr (sizeof...(ComponentTypes) > 0)
		{
			INTERNAL_AssignMultipleComponentsWithoutChecks<ComponentTypes...>(_entityIndex);
		}
	}

	template<typename FirstComponent, typename... ComponentTypes>
	int INTERNAL_RemoveComponentsWithoutChecks(EntityID _entityId)
	{
		unsigned int entityIndex = ECS::GetIndexFromId(_entityId);
		unsigned int componentIndex = GetComponentIndex<FirstComponent>();

		// We check if this Entity might not have this component enabled.
		if (!INTERNAL_IsComponentEnabledWithoutChecks(entityIndex, componentIndex))
		{
			assert(false && "Trying to remove a component from an entity that didn't have it enabled.");

			return -2;
		}

		reinterpret_cast<FirstComponent*>(m_componentPools[componentIndex]->GetElement(entityIndex))->~FirstComponent();
		m_entities[entityIndex].m_componentMask.reset(componentIndex);

		if constexpr (sizeof...(ComponentTypes) > 0)
		{
			return INTERNAL_RemoveComponentsWithoutChecks<ComponentTypes...>(entityIndex);
		}
		else
		{
			return 0;
		}
	}
	template<typename FirstComponent, typename... ComponentTypes>
	int INTERNAL_RemoveComponentsWithoutChecks(unsigned int _entityIndex)
	{
		unsigned int componentIndex = GetComponentIndex<FirstComponent>();

		// We check if this Entity might not have this component enabled.
		if (!INTERNAL_IsComponentEnabledWithoutChecks(_entityIndex, componentIndex))
		{
			assert(false && "Trying to remove a component from an entity that didn't have it enabled.");

			return -2;
		}

		reinterpret_cast<FirstComponent*>(m_componentPools[componentIndex]->GetElement(_entityIndex))->~FirstComponent();
		m_entities[_entityIndex].m_componentMask.reset(componentIndex);

		if constexpr (sizeof...(ComponentTypes) > 0)
		{
			return INTERNAL_RemoveComponentsWithoutChecks<ComponentTypes...>(_entityIndex);
		}
		else
		{
			return 0;
		}
	}
	int INTERNAL_RemoveComponentWithoutChecks(unsigned int _entityIndex, unsigned int _componentIndex);

	template<typename T>
	inline T* INTERNAL_GetComponentWithoutChecks(const EntityID _entityId) const
	{
		return reinterpret_cast<T*>(m_componentPools[GetComponentIndex<T>()]->GetElement(ECS::GetIndexFromId(_entityId)));
	}
	template<typename T>
	inline T* INTERNAL_GetComponentWithoutChecks(const unsigned int _entityIndex) const
	{
		return reinterpret_cast<T*>(m_componentPools[GetComponentIndex<T>()]->GetElement(_entityIndex));
	}
	inline void* INTERNAL_GetComponentWithoutChecks(const unsigned int _entityIndex, const unsigned int _componentIndex) const
		{	return m_componentPools[_componentIndex]->GetElement(_entityIndex); };

#pragma endregion

#pragma region Iterator
public:

	struct PoolIterator
	{
	private:
		bool m_emptyMask{ false };
		ECS_EntityPool* m_pEntityPool{ nullptr };
		EntityComponentMask m_componentMask;
		unsigned int m_uCurrentEntityIndex{ 0 };

	public:

		/// <summary>
		/// Invalid Iterator, use only internally.
		/// </summary>
		PoolIterator() {};

		template<typename... IteratorTypes>
		PoolIterator(ECS_EntityPool* _pEntityPool, unsigned int _uCurrentEntityIndex)
			: m_pEntityPool{ _pEntityPool }, m_uCurrentEntityIndex{ _uCurrentEntityIndex }
		{
			if constexpr (sizeof...(IteratorTypes) > 0)
			{
				assert(m_pEntityPool->HaveComponentsBeenInitialized<IteratorTypes...>() && "Cannot create an iterator with Types that are different from those that initialized the Entity Pool.");

				m_componentMask = m_pEntityPool->ConvertPoolMaskToEntityMask<IteratorTypes...>();
			}
			else
			{
				m_emptyMask = true;
			}
		}

		PoolIterator(
			ECS_EntityPool* _pEntityPool,
			unsigned int _uCurrentEntityIndex,
			EntityComponentMask _componentMask,
			bool _isMaskEmpty = false)
			: m_pEntityPool{ _pEntityPool },
			m_uCurrentEntityIndex{ _uCurrentEntityIndex },
			m_componentMask{ _componentMask },
			m_emptyMask{ _isMaskEmpty }
		{};

		PoolIterator(const PoolIterator& _other)
			: m_pEntityPool{ _other.m_pEntityPool },
			m_uCurrentEntityIndex{ _other.m_uCurrentEntityIndex },
			m_componentMask{ _other.m_componentMask },
			m_emptyMask{ _other.m_emptyMask }
		{};

		EntityID operator*() const;

		bool operator==(const PoolIterator& other) const;
		bool operator!=(const PoolIterator& other) const;

		PoolIterator& operator++();
		PoolIterator& operator--();

		inline static PoolIterator GetEmptyIterator() { return PoolIterator(); };
		inline bool IsIteratorInvalid() const { return m_pEntityPool == nullptr; };

		bool IsCurrentIndexValidForIterator() const;
		inline unsigned int GetCurrentEntityIndex() const
		{
			return m_uCurrentEntityIndex;
		}

		template<typename T>
		T* GetComponent()
		{
			const unsigned int uComponentIndex = m_pEntityPool->GetComponentIndex<T>();

			if (!m_pEntityPool->HasComponentBeenInitialized(uComponentIndex))
			{
				assert(false && "Trying to Get a component that wasn't initialized in this Entity Pool.");

				return nullptr;
			}

			if (m_uCurrentEntityIndex >= m_pEntityPool->m_entities.size())
			{
				assert(false && "Trying to Get a component from a PoolIterator with an invalid entity Index.");

				return nullptr;
			}

			return m_pEntityPool->INTERNAL_GetComponentWithoutChecks<T>(m_uCurrentEntityIndex);
		}
		inline void* GetComponent(const ComponentIndex _componentIndex) 
			{	return m_pEntityPool->GetComponent(m_uCurrentEntityIndex, _componentIndex); };
		template<typename T>
		inline bool HasComponentEnabled() const
		{
			return m_pEntityPool->HasComponentBeenInitialized<T>() && m_pEntityPool->HasComponentEnabled<T>(m_uCurrentEntityIndex);
		}
		inline bool HasComponentEnabled(const ComponentIndex _componentIndex) const
			{	return m_pEntityPool->HasComponentEnabled(m_uCurrentEntityIndex, _componentIndex); };
	};

public:
	template<typename... IteratorTypes>
	PoolIterator BeginIterator()
	{
		unsigned int firstIndex = 0;
		EntityComponentMask mask;

		if constexpr (sizeof...(IteratorTypes) > 0)
		{
			mask = ConvertPoolMaskToEntityMask<IteratorTypes...>();
		}

		while (firstIndex < m_entities.size() &&
			(mask != (mask & m_entities[firstIndex].m_componentMask) || IsEntityDeleted(firstIndex))
			)
		{
			firstIndex++;
		}

		return PoolIterator(this, firstIndex, mask, sizeof...(IteratorTypes) == 0);
	}
	PoolIterator BeginIterator(const PoolComponentMask& _poolMask, bool _emptyMask = false);

	template<typename... IteratorTypes>
	PoolIterator EndIterator()
	{
		unsigned int lastIndex = m_entities.size();

		EntityComponentMask mask;

		if constexpr (sizeof...(IteratorTypes) > 0)
		{
			mask = ConvertPoolMaskToEntityMask<IteratorTypes...>();
		}

		return PoolIterator(this, lastIndex, mask, sizeof...(IteratorTypes) == 0);
	}
	PoolIterator EndIterator(const PoolComponentMask& _poolMask, bool _emptyMask);

#pragma endregion
};