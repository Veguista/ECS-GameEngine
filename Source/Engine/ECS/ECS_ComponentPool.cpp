#include "ECS_ComponentPool.h"
#include "ECS_Configuration.h"
#include <assert.h>

ECS_ComponentPool::ECS_ComponentPool(
	unsigned int _componentSize,
	unsigned int _maxNumberOfEntities,
	delayed_updater_func _delayedUpdaterFunct,
	delayed_constructor_func _delayedConstructorFunct,
	delayed_destructor_func _delayedDeleterFunct,
	delayed_copy_constructor_func _delayedCopyConstructorFunct,
	delayed_funct_plus_one_object_param _delayedFunctWithOneObjectParam,
	delayed_funct_serialize _delayedFunctSerialize,
	delayed_funct_serialize _delayedFunctLoad)
	: m_uComponentSize{ _componentSize },
	m_uNumberOfEntities{ _maxNumberOfEntities },
	m_delayedUpdaterFunct{ _delayedUpdaterFunct },
	m_delayedConstructorFunct{ _delayedConstructorFunct },
	m_delayedDeleterFunct{ _delayedDeleterFunct },
	m_delayedCopyConstructorFunct{ _delayedCopyConstructorFunct },
	m_delayedFunctWithOneObjectParam{ _delayedFunctWithOneObjectParam },
	m_delayedFunctSerialize{ _delayedFunctSerialize },
	m_delayedFunctLoad{ _delayedFunctLoad }
{
	pData = new char[m_uComponentSize * m_uNumberOfEntities];
}

ECS_ComponentPool::~ECS_ComponentPool()
{
	delete[] pData;
}

void* ECS_ComponentPool::GetElement(unsigned int _index) const
{
	assert(_index < m_uNumberOfEntities && "Cannot obtain an index that is bigger than the number of entities of the Entity Pool.");

	return &(pData[m_uComponentSize * _index]);
}
void* ECS_ComponentPool::GetElement(EntityID _entityId) const
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot obtain an Entity index that is bigger than the number of entities of the Entity Pool.");

	return &(pData[m_uComponentSize * ECS::GetIndexFromId(_entityId)]);
}

int ECS_ComponentPool::CalculateElementIndex(const void* _pointer) const
{
	int result = static_cast<int>((static_cast<const char*>(_pointer) - pData) / m_uComponentSize);
	if (result < m_uNumberOfEntities)
	{
		return result;
	}

	return -1;
}

void ECS_ComponentPool::UpdateElement(unsigned int _index, float _deltaTime)
{
	assert(_index < m_uNumberOfEntities && "Cannot create a Update a Component at a bigger index than the number of entities of the Entity Pool.");
	assert(m_delayedUpdaterFunct != nullptr && "Trying to call the Update function of an element but the pointer to the function was nullptr.");

	m_delayedUpdaterFunct(GetElement(_index), _deltaTime);
}
void ECS_ComponentPool::UpdateElement(EntityID _entityId, float _deltaTime)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot create a Update a Component at a bigger Entity index than the number of entities of the Entity Pool.");
	assert(m_delayedUpdaterFunct != nullptr && "Trying to call the Update function of an Entity but the pointer to the function was nullptr.");

	m_delayedUpdaterFunct(GetElement(_entityId), _deltaTime);
}
void ECS_ComponentPool::UpdateElements(unsigned int* _arrayOfIndex, unsigned int _arrayLength, float _deltaTime)
{
	assert(_arrayOfIndex != nullptr && "Trying to Update the component of multiple indexes but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to Update the component of multiple indexes but the number of indexes is 0.");
	assert(m_delayedUpdaterFunct != nullptr && "Trying to Update the component of multiple indexes but the pointer to the original Update method was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedUpdaterFunct(GetElement(_arrayOfIndex[i]), _deltaTime);
	}
}
void ECS_ComponentPool::UpdateElements(EntityID* _arrayOfEntityIds, unsigned int _arrayLength, float _deltaTime)
{
	assert(_arrayOfEntityIds != nullptr && "Trying to Update the component of multiple Entities but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to Update the component of multiple Entities but the number of Entities is 0.");
	assert(m_delayedUpdaterFunct != nullptr && "Trying to Update the component of multiple Entities but the pointer to the original Update method was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedUpdaterFunct(GetElement(_arrayOfEntityIds[i]), _deltaTime);
	}
}

void* ECS_ComponentPool::CreateElement(unsigned int _index)
{
	assert(_index < m_uNumberOfEntities && "Cannot create a Component at an index bigger than the number of entities of the Entity Pool.");
	assert(m_delayedConstructorFunct != nullptr && "Trying to call the Constructor of an element but the pointer to the function was nullptr.");

	return m_delayedConstructorFunct(GetElement(_index));
}
void* ECS_ComponentPool::CreateElement(EntityID _entityId)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot create a Component at an Entity index bigger than the number of entities of the Entity Pool.");
	assert(m_delayedConstructorFunct != nullptr && "Trying to call the Constructor of an Entity component but the pointer to the function was nullptr.");

	return m_delayedConstructorFunct(GetElement(_entityId));
}
void ECS_ComponentPool::CreateElements(unsigned int* _arrayOfIndex, unsigned int _arrayLength)
{
	assert(_arrayOfIndex != nullptr && "Trying to assign a component to multiple indexes but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to assign a component to multiple indexes but the number of indexes is 0.");
	assert(m_delayedConstructorFunct != nullptr && "Trying to call the Constructor of an Entity component but the pointer to the function was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedConstructorFunct(GetElement(_arrayOfIndex[i]));
	}
}
void ECS_ComponentPool::CreateElements(EntityID* _arrayOfEntityIds, unsigned int _arrayLength)
{
	assert(_arrayOfEntityIds != nullptr && "Trying to assign a component to multiple indexes but the Entity array is nullptr.");
	assert(_arrayLength > 0 && "Trying to assign a component to multiple Entities but the number of Entities in the array is 0.");
	assert(m_delayedConstructorFunct != nullptr && "Trying to call the Constructor of an Entity component but the pointer to the function was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedConstructorFunct(GetElement(_arrayOfEntityIds[i]));
	}
}

void* ECS_ComponentPool::CreateElementWithCopyConstructor(unsigned int _index, const void* _elementToCopy)
{
	assert(_index < m_uNumberOfEntities && "Cannot create a Component at an index bigger than the number of entities of the Entity Pool.");
	assert(_elementToCopy != nullptr && "Trying to call the Copy Constructor of an element but the pointer to the original was nullptr.");
	assert(m_delayedCopyConstructorFunct != nullptr && "Trying to call the Copy Constructor of an element but the pointer to the function was nullptr.");

	return m_delayedCopyConstructorFunct(GetElement(_index), _elementToCopy);
}
void* ECS_ComponentPool::CreateElementWithCopyConstructor(EntityID _entityId, const void* _elementToCopy)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot create a Component at an Entity index bigger than the number of entities of the Entity Pool.");
	assert(_elementToCopy != nullptr && "Trying to call the Copy Constructor of an Entity Component but the pointer to the original was nullptr.");
	assert(m_delayedCopyConstructorFunct != nullptr && "Trying to call the Copy Constructor of an Entity Component but the pointer to the function was nullptr.");

	return m_delayedCopyConstructorFunct(GetElement(_entityId), _elementToCopy);
}
void ECS_ComponentPool::CreateElementsWithCopyConstructor(unsigned int* _arrayOfIndex, unsigned int _arrayLength, const void* _elementToCopy)
{
	assert(_arrayOfIndex != nullptr && "Trying to assign a component to multiple indexes but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to assign a component to multiple indexes but the number of indexes is 0.");
	assert(_elementToCopy != nullptr && "Trying to call the Copy Constructor of an element but the pointer to the original was nullptr.");
	assert(m_delayedCopyConstructorFunct != nullptr && "Trying to call the Copy Constructor of an Entity Component but the pointer to the function was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedCopyConstructorFunct(GetElement(_arrayOfIndex[i]), _elementToCopy);
	}
}
void ECS_ComponentPool::CreateElementsWithCopyConstructor(EntityID* _arrayOfEntityIds, unsigned int _arrayLength, const void* _elementToCopy)
{
	assert(_arrayOfEntityIds != nullptr && "Trying to assign a component to multiple Entities but the EntityId array is nullptr.");
	assert(_arrayLength > 0 && "Trying to assign a component to multiple Entities but the number of Entities is 0.");
	assert(_elementToCopy != nullptr && "Trying to call the Copy Constructor of an Entity Component but the pointer to the original was nullptr.");
	assert(m_delayedCopyConstructorFunct != nullptr && "Trying to call the Copy Constructor of an Entity Component but the pointer to the function was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedCopyConstructorFunct(GetElement(_arrayOfEntityIds[i]), _elementToCopy);
	}
}

void ECS_ComponentPool::CallElementDestructor(unsigned int _index)
{
	assert(_index < m_uNumberOfEntities && "Cannot call the destructor of a Component with an index that is bigger than the number of entities of the Entity Pool.");
	assert(m_delayedDeleterFunct != nullptr && "Trying to call the Destructor of an element but the pointer to the function was nullptr.");

	m_delayedDeleterFunct(GetElement(_index));
}
void ECS_ComponentPool::CallElementDestructor(EntityID _entityId)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot call the destructor of an Entity Component with an Entity Index that is bigger than the number of entities of the Entity Pool.");
	assert(m_delayedDeleterFunct != nullptr && "Trying to call the Destructor of an element but the pointer to the function was nullptr.");

	m_delayedDeleterFunct(GetElement(_entityId));
}
void ECS_ComponentPool::CallElementsDestructor(unsigned int* _arrayOfIndex, unsigned int _arrayLength)
{
	assert(_arrayOfIndex != nullptr && "Trying to call the destructor of multiple components but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to call the destructor of multiple components but the number of indexes is 0.");
	assert(m_delayedDeleterFunct != nullptr && "Trying to call the destructor of multiple components but the pointer to the Destructor method was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedDeleterFunct(GetElement(_arrayOfIndex[i]));
	}
}
void ECS_ComponentPool::CallElementsDestructor(EntityID* _arrayOfEntityIds, unsigned int _arrayLength)
{
	assert(_arrayOfEntityIds != nullptr && "Trying to call the destructor of multiple components but the EntityId array is nullptr.");
	assert(_arrayLength > 0 && "Trying to call the destructor of multiple components but the number of EntityIds is 0.");
	assert(m_delayedDeleterFunct != nullptr && "Trying to call the destructor of multiple components but the pointer to the Destructor method was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedDeleterFunct(GetElement(_arrayOfEntityIds[i]));
	}
}

void ECS_ComponentPool::CallStoredFunctionWithObjectParam(unsigned int _index, void* _object)
{
	assert(_index < m_uNumberOfEntities && "Cannot call the stored function of Component at an index bigger than the number of entities of the Entity Pool.");
	assert(_object != nullptr && "Trying to call the Stored function of an element but the pointer to the object was nullptr.");
	assert(m_delayedFunctWithOneObjectParam != nullptr && "Trying to call the Stored function of an element but the pointer to the function was nullptr.");

	m_delayedFunctWithOneObjectParam(GetElement(_index), _object);
}
void ECS_ComponentPool::CallStoredFunctionWithObjectParam(EntityID _entityId, void* _object)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot call the stored function of Component at an Entity Index bigger than the number of entities of the Entity Pool.");
	assert(_object != nullptr && "Trying to call the Stored function of an Entity Component but the pointer to the object was nullptr.");
	assert(m_delayedFunctWithOneObjectParam != nullptr && "Trying to call the Stored function of an Entity Component but the pointer to the function was nullptr.");

	m_delayedFunctWithOneObjectParam(GetElement(_entityId), _object);
}
void ECS_ComponentPool::CallStoredFunctionWithObjectParam(unsigned int* _arrayOfIndex, unsigned int _arrayLength, void* _object)
{
	assert(_object != nullptr && "Trying to call the Stored function of multiple Entity Components but the pointer to the object was nullptr.");
	assert(_arrayOfIndex != nullptr && "Trying to call the Stored function of multiple components but the index array is nullptr.");
	assert(_arrayLength > 0 && "Trying to call the Stored function of multiple components but the number of indexes is 0.");
	assert(m_delayedFunctWithOneObjectParam != nullptr && "Trying to call the Stored function of multiple components but the pointer to the method was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedFunctWithOneObjectParam(GetElement(_arrayOfIndex[i]), _object);
	}
}
void ECS_ComponentPool::CallStoredFunctionWithObjectParam(EntityID* _arrayOfEntityIds, unsigned int _arrayLength, void* _object)
{
	assert(_object != nullptr && "Trying to call the Stored function of multiple Entity Components but the pointer to the object was nullptr.");
	assert(_arrayOfEntityIds != nullptr && "Trying to call the Stored function of multiple components but the Entity array is nullptr.");
	assert(_arrayLength > 0 && "Trying to call the Stored function of multiple components but the number of Entities is 0.");
	assert(m_delayedFunctWithOneObjectParam != nullptr && "Trying to call the Stored function of multiple components but the pointer to the method was nullptr.");

	for (unsigned int i = 0; i < _arrayLength; i++)
	{
		m_delayedFunctWithOneObjectParam(GetElement(_arrayOfEntityIds[i]), _object);
	}
}

bool ECS_ComponentPool::SerializeElement(unsigned int _index, pugi::xml_node* _ComponentNode)
{
	assert(_index < m_uNumberOfEntities && "Cannot Serialize a Component at an index bigger than the number of entities of the Entity Pool.");
	assert(_ComponentNode != nullptr && "Trying to Serialize an element but the pointer to the ComponentNode was nullptr.");
	assert(m_delayedFunctSerialize != nullptr && "Trying to Serialize an element but the pointer to the function was nullptr.");

	return m_delayedFunctSerialize(GetElement(_index), _ComponentNode);
}
bool ECS_ComponentPool::SerializeElement(EntityID _entityId, pugi::xml_node* _ComponentNode)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot Serialize a Component at an index bigger than the number of entities of the Entity Pool.");
	assert(_ComponentNode != nullptr && "Trying to Serialize an element but the pointer to the ComponentNode was nullptr.");
	assert(m_delayedFunctSerialize != nullptr && "Trying to Serialize an element but the pointer to the function was nullptr.");

	return m_delayedFunctSerialize(GetElement(_entityId), _ComponentNode);
}

bool ECS_ComponentPool::LoadElement(unsigned int _index, pugi::xml_node* _ComponentNode)
{
	assert(_index < m_uNumberOfEntities && "Cannot Load a Component at an index bigger than the number of entities of the Entity Pool.");
	assert(_ComponentNode != nullptr && "Trying to Load an element but the pointer to the ComponentNode was nullptr.");
	assert(m_delayedFunctLoad != nullptr && "Trying to Load an element but the pointer to the function was nullptr.");

	return m_delayedFunctLoad(GetElement(_index), _ComponentNode);
}
bool ECS_ComponentPool::LoadElement(EntityID _entityId, pugi::xml_node* _ComponentNode)
{
	assert(ECS::GetIndexFromId(_entityId) < m_uNumberOfEntities && "Cannot Load a Component at an index bigger than the number of entities of the Entity Pool.");
	assert(_ComponentNode != nullptr && "Trying to Load an element but the pointer to the ComponentNode was nullptr.");
	assert(m_delayedFunctLoad != nullptr && "Trying to Load an element but the pointer to the function was nullptr.");

	return m_delayedFunctLoad(GetElement(_entityId), _ComponentNode);
}
