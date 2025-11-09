#pragma once

#include "ECS_Configuration.h"
#include "ECS_MACROS.h"
#include "ECS_EntityPool.h"
#include "Engine/Engine.h"
#include "Engine/Util/XML/XML_File_Handler.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include <typeinfo>
#include <assert.h>
#include <vector>
#include <bitset>
#include <string>

class Engine;
class ECS_EntityPool;

class ECS_PoolManager
{
	pugi::xml_document PoolInfoDocument;

	std::vector<ECS_EntityPool> m_pools;
	std::vector<std::bitset<MAX_TOTAL_NUMBER_OF_COMPONENTS>> m_componentsInEachPool;

	// Vectors storing which components implement which Interfaces.
	PoolComponentMask m_IUpdateComponentIds;
	PoolComponentMask m_ITransformComponentIds;
	PoolComponentMask m_IRenderComponentIds;
	PoolComponentMask m_ISerializableComponentIds;

	static inline ECS_PoolManager* Instance{ nullptr };

	ECS_PoolManager();

#pragma region Manager General Methods

public:
	static ECS_PoolManager* InitManager();
	inline static ECS_PoolManager* GetInstance() {	return Instance; };
	static void DestroyInstance();

	inline unsigned int HowManyInitializedEPools() const {	return m_pools.size(); };
	inline static unsigned int HowManyInitializedComponents() { return ECS_INTERNAL::NumberOfInitializedComponents; };

#pragma endregion

#pragma region Entity Management

public:
	inline bool IsEntityDeleted(const EntityID _id) const {	return m_pools[ECS::GetPoolFromId(_id)].IsEntityDeleted(_id); };

	inline EntityID CreateEntity(const PoolID _poolId) {	return GetEntityPool(_poolId)->CreateEntity(); };
	/// <summary>
	/// Tries to create a set number of entities at the given Entity pool.
	/// It also stores the newly created EntityIDs in a given EntityID-buffer.
	/// </summary>
	/// <param name="_numberOfEntitiesToCreate">: How many entities we want to create.</param>
	/// <param name="_entitiesIdBuffer">: Buffer in which to store the newly generated EntityIDs. 
	/// Must be of size similar or bigger than the number of Entities we are trying to generate.</param>
	/// <returns>The number of Entities that were successfully created.</returns>
	inline unsigned int CreateEntities(const PoolID _poolId, const unsigned int _numberOfEntitiesToCreate, EntityID* _entitiesIdBuffer)
		{ return GetEntityPool(_poolId)->CreateEntities(_numberOfEntitiesToCreate, _entitiesIdBuffer); }

	template<typename... Components>
	inline EntityID CreateEntityWithComponents(const PoolID _poolId)
	{
		return GetEntityPool(_poolId)->CreateEntityWithComponents<Components...>();
	}

	inline void DestroyEntity(const EntityID _entityId) { GetEntityPool(ECS::GetPoolFromId(_entityId))->DestroyEntity(_entityId); };
	void DestroyEntities(EntityID* _entityIdBuffer, unsigned int _lengthOfBuffer);

#pragma endregion

#pragma region Component Management
public:
	template<typename FirstComponent, typename... OtherComponents>
	void StoreComponentInterfaces()
	{
		if constexpr (ECS_INTERNAL::Implements_IECS_Update<FirstComponent>())
		{
			m_IUpdateComponentIds.set(ECS::GetComponentId<FirstComponent>());
		}
		if constexpr (ECS_INTERNAL::Implements_IECS_Transform<FirstComponent>())
		{
			m_ITransformComponentIds.set(ECS::GetComponentId<FirstComponent>());
		}
		if constexpr (ECS_INTERNAL::Implements_IECS_Render<FirstComponent>())
		{
			m_IRenderComponentIds.set(ECS::GetComponentId<FirstComponent>());
		}
		if constexpr (ECS_INTERNAL::Implements_ECS_Serialization<FirstComponent>())
		{
			m_ISerializableComponentIds.set(ECS::GetComponentId<FirstComponent>());
		}

		if constexpr(sizeof...(OtherComponents) > 0)
		{
			StoreComponentInterfaces<OtherComponents...>();
		}
	}

	void UpdateComponents(float _deltaTime);
	void RenderEntities();

	template<typename Component>
	EntityID FindComponentOwnerEntity(Component* _component)
	{
		auto iterator = BeginIterator<Component>();
		while (iterator != EndIterator<Component>())
		{
			if (iterator->GetComponent<Component>() == _component)
			{
				return *iterator;
			}

			iterator++;
		}
	}
	EntityID FindComponentOwnerEntity(void* _component) const;

	inline bool IsComponentSerializable(const ComponentIndex _componentIndex) const { return m_ISerializableComponentIds.test(_componentIndex); };

#pragma endregion

#pragma region Pool Management

public:
	template<typename... ComponentTypes>
	PoolID CreateEntityPool(unsigned int _maxNumberOfEntities, const std::string& _PoolName)
	{
		if constexpr (sizeof...(ComponentTypes) == 0)
		{
			assert(false && "Trying to create an Entity Pool with 0 allowed Entity Components.");
			return;
		}

		assert(_maxNumberOfEntities > 0 && "Trying to create an Entity Pool with 0 maximum Entities.");

		PoolID newPoolId = HowManyInitializedEPools();

		assert(newPoolId < ECS::CONSTANTS::InvalidPoolId() && "Cannot create more than the maximum number of Entity Pools allowed (4095).");

		// Initializing T IDs.
		ECS::InitializeAnyNewComponents<ComponentTypes...>();

		StoreComponentInterfaces<ComponentTypes...>();

		// Creating a new Valid Components Mask for this new Entity Pool.
		PoolComponentMask newComponentMask;
		ECS::SetPoolComponentMask<ComponentTypes...>(newComponentMask);
		m_componentsInEachPool.push_back(newComponentMask);

		// Creating the Pool.
		m_pools.push_back(ECS_EntityPool());
		void* locationOfNewPool = reinterpret_cast<void*>(&m_pools.data()[m_pools.size() - 1]);
		ECS_EntityPool::CreateEntityPoolAtLocation<ComponentTypes...>(locationOfNewPool, this, _maxNumberOfEntities, newPoolId);

		// Extending our Pool Info Document
		pugi::xml_node poolNode = PoolInfoDocument.child("PoolList").append_child("EntityPool");
		poolNode.append_attribute("PoolName").set_value(_PoolName);
		poolNode.append_attribute("PoolID").set_value(std::to_string(newPoolId));

		pugi::xml_node componentsListNode = poolNode.append_child("Components");
		CreatePoolInfoInXMLDocument<ComponentTypes...>(componentsListNode, reinterpret_cast<ECS_EntityPool*>(locationOfNewPool));

		// Saving the XML Document.
		std::string xml_document_filePath = Engine::CONFIGURATION_FILES_PATH;
		xml_document_filePath.append(Engine::POOL_FILE_NAME);
		xml_document_filePath.append(Engine::XML_FILE_EXTENSION);

		XML_UTIL::SaveXMLFile(xml_document_filePath.c_str(), PoolInfoDocument);

		return newPoolId;
	}

	template<typename FirstComponent, typename... OtherComponents>
	void CreatePoolInfoInXMLDocument(pugi::xml_node& _componentsListNode, const ECS_EntityPool* _EntityPool)
	{
		pugi::xml_node componentNode = _componentsListNode.append_child("ECS_Component");
		componentNode.append_attribute("ComponentName").set_value(typeid(std::remove_cvref_t<FirstComponent>).name());
		componentNode.append_attribute("ComponentIndex").set_value(std::to_string(_EntityPool->GetComponentIndex<std::remove_cvref_t<FirstComponent>>()).c_str());

		if constexpr (sizeof...(OtherComponents) > 0)
		{
			CreatePoolInfoInXMLDocument<OtherComponents...>(_componentsListNode, _EntityPool);
		}
	}

	ECS_EntityPool* GetEntityPool(PoolID _poolId);

	inline const pugi::xml_node& GetPoolListNode() { return PoolInfoDocument.child("PoolList"); };

#pragma endregion

#pragma region Iterator

public:
	struct Iterator
	{
	private:
		ECS_PoolManager* m_pPoolManager{ nullptr };
		PoolComponentMask m_componentMask;
		bool m_emptyMask{ false };

		ECS_EntityPool::PoolIterator m_currentBeginIterator;
		ECS_EntityPool::PoolIterator m_currentActiveIterator;
		ECS_EntityPool::PoolIterator m_currentEndIterator;

	public:
		unsigned int m_uCurrentPoolId{ 0 };


		Iterator(ECS_PoolManager* _pPoolManager, unsigned int _uCurrentPoolId, PoolComponentMask _componentMask, bool _emptyMask = false);

		template<typename... IteratorTypes>
		static Iterator CreateIterator(ECS_PoolManager* _pPoolManager, unsigned int _uCurrentPoolId)
		{
			PoolComponentMask componentMask;

			if constexpr (sizeof...(IteratorTypes) > 0)
			{
				assert(ECS::HaveComponentsBeenInitialized<IteratorTypes...>() && "Cannot create an iterator with filter types that have not been used to initialize any Entity Pools.");

				ECS::SetPoolComponentMask<IteratorTypes...>(componentMask);

				return Iterator(_pPoolManager, _uCurrentPoolId, componentMask, false);
			}
			else
			{
				return Iterator(_pPoolManager, _uCurrentPoolId, componentMask, true);
			}
		}
		
		Iterator(const Iterator& _other) 
			: m_pPoolManager{ _other.m_pPoolManager }, 
			m_uCurrentPoolId{ _other.m_uCurrentPoolId },
			m_componentMask{ _other.m_componentMask }, 
			m_emptyMask{ _other.m_emptyMask },
			m_currentBeginIterator{ _other.m_currentBeginIterator },
			m_currentActiveIterator{ _other.m_currentActiveIterator }, 
			m_currentEndIterator{ _other.m_currentEndIterator }
		{
		};

		EntityID operator*() const
		{
			return *m_currentActiveIterator;
		}

		bool operator==(const Iterator& _other) const
		{
			return (m_uCurrentPoolId == _other.m_uCurrentPoolId)
				&& (m_uCurrentPoolId == ECS_INTERNAL::NumberOfInitializedComponents || m_currentActiveIterator == _other.m_currentActiveIterator);
			// If we are at the end of the iterator                                OR both iterators are at the same point.
		}
		bool operator!=(const Iterator& _other) const
		{
			return (m_uCurrentPoolId != _other.m_uCurrentPoolId) ||
				(m_uCurrentPoolId != ECS_INTERNAL::NumberOfInitializedComponents && m_currentActiveIterator != _other.m_currentActiveIterator);
			// If we are not at the end of the iterator                            AND  both iterators are not at the same point.
		}

		Iterator operator++()
		{
			if(m_currentActiveIterator != m_currentEndIterator)
				++m_currentActiveIterator;

			while (m_currentActiveIterator == m_currentEndIterator)
			{
				m_uCurrentPoolId++;

				// If we reached our end of pools.
				if (m_uCurrentPoolId >= m_pPoolManager->HowManyInitializedEPools())
				{
					return *this;
				}
				if (DoesEntityPoolImplementAllNecessaryComponents(m_uCurrentPoolId))
				{
					m_currentBeginIterator = m_pPoolManager->GetEntityPool(m_uCurrentPoolId)->BeginIterator(m_componentMask, m_emptyMask);
					m_currentActiveIterator = ECS_EntityPool::PoolIterator(m_currentBeginIterator);
					m_currentEndIterator = m_pPoolManager->GetEntityPool(m_uCurrentPoolId)->EndIterator(m_componentMask, m_emptyMask);
				}
				// Else, we don't do anything and we will move on to the next Pool for sure.
			}
		}
		Iterator operator--()
		{
			if (m_currentActiveIterator != m_currentBeginIterator)
			{
				--m_currentActiveIterator;
			}
			else
			{
				unsigned int newPoolId = m_uCurrentPoolId;

				do
				{
					// If we reached the beginning of our first pool without finding a valid element.
					if (newPoolId == 0)
					{
						return *this;
					}

					newPoolId--;

					if (DoesEntityPoolImplementAllNecessaryComponents(m_uCurrentPoolId))
					{
						m_uCurrentPoolId = newPoolId;

						m_currentBeginIterator = m_pPoolManager->GetEntityPool(m_uCurrentPoolId)->BeginIterator(m_componentMask, m_emptyMask);
						m_currentEndIterator = m_pPoolManager->GetEntityPool(m_uCurrentPoolId)->EndIterator(m_componentMask, m_emptyMask);
						m_currentActiveIterator = ECS_EntityPool::PoolIterator(m_currentEndIterator);
					}
					// Else, we don't do anything and we will move on to the next Pool for sure.
				}
				while (m_currentBeginIterator == m_currentEndIterator);
			}
		}

		inline bool IsCurrentIteratorValid() const
		{
			return !m_currentBeginIterator.IsIteratorInvalid() && (m_currentBeginIterator != m_currentEndIterator);
		}
		
		inline unsigned int GetCurrentEntityIndex() const
		{
			return m_currentActiveIterator.GetCurrentEntityIndex();
		}

		inline void AdvanceToEndOfEntityPool()
		{
			m_currentActiveIterator = m_currentEndIterator;
		}

		template<typename T>
		inline T* GetComponent()
		{			
			return m_currentActiveIterator.GetComponent<T>();
		}
		void* GetComponent(ComponentIndex _componentIndex);
		template<typename T>
		inline bool HasComponentEnabled() const
			{ return IsCurrentIteratorValid() && m_currentActiveIterator.HasComponentEnabled<T>(); }
		inline bool HasComponentEnabled(const ComponentIndex _componentIndex) const
			{ return IsCurrentIteratorValid() && m_currentActiveIterator.HasComponentEnabled(_componentIndex); };

	private:
		bool DoesEntityPoolImplementAllNecessaryComponents(const unsigned int _poolId) const
		{
			if (m_emptyMask)
			{
				return true;
			}
			else
			{
				return m_componentMask == (m_componentMask & m_pPoolManager->m_componentsInEachPool[_poolId]);
			}
		}
	};

	template<typename... IteratorTypes>
	Iterator BeginIterator()
	{
		if constexpr (sizeof...(IteratorTypes) > 0)
		{
			if (!ECS::HaveComponentsBeenInitialized<IteratorTypes...>())
			{
				return EndIterator<IteratorTypes...>();
			}
		}

		Iterator resultIterator = Iterator::CreateIterator<IteratorTypes...>(this, 0);

		if (!resultIterator.IsCurrentIteratorValid())
		{
			++resultIterator;
		}

		return resultIterator;
	}
	Iterator BeginIterator(const PoolComponentMask& _mask);

	template <typename... IteratorTypes>
	Iterator EndIterator()
	{
		if constexpr (sizeof...(IteratorTypes) > 0)
		{
			return Iterator::CreateIterator<IteratorTypes...>(this, HowManyInitializedEPools());
		}
		else
		{
			return Iterator::CreateIterator(this, HowManyInitializedEPools());
		}
	}
	inline Iterator EndIterator(const PoolComponentMask& _mask)
		{	return Iterator(this, HowManyInitializedEPools(), _mask, false); };

#pragma endregion

};