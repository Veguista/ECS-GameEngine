#pragma once

#include "ECS_Typedefs.h"
#include "ECS_EntityID.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"


struct ECS_ComponentPool
{
private:
	char* pData{ nullptr };

public:
	const unsigned int m_uNumberOfEntities;
	const unsigned int m_uComponentSize;
	delayed_updater_func m_delayedUpdaterFunct;
	delayed_constructor_func m_delayedConstructorFunct;
	delayed_destructor_func m_delayedDeleterFunct;
	delayed_copy_constructor_func m_delayedCopyConstructorFunct;
	delayed_funct_plus_one_object_param m_delayedFunctWithOneObjectParam;
	delayed_funct_serialize m_delayedFunctSerialize;
	delayed_funct_serialize m_delayedFunctLoad;

	// Constructors & Destructors
	ECS_ComponentPool(
		unsigned int _componentSize,
		unsigned int _maxNumberOfEntities,
		delayed_updater_func _delayedUpdaterFunct,
		delayed_constructor_func _delayedConstructorFunct,
		delayed_destructor_func _delayedDeleterFunct,
		delayed_copy_constructor_func _delayedCopyConstructorFunct,
		delayed_funct_plus_one_object_param _delayedFunctWithOneObjectParam,
		delayed_funct_serialize _delayedFunctSerialize,
		delayed_funct_serialize _delayedFunctLoad);
	~ECS_ComponentPool();

	// Public Methods
	void* GetElement(unsigned int _index) const;
	void* GetElement(EntityID _entityId) const;
	int CalculateElementIndex(const void* _pointer) const;

	void UpdateElement(unsigned int _index, float _deltaTime);
	void UpdateElement(EntityID _entityId, float _deltaTime);
	void UpdateElements(unsigned int* _arrayOfIndex, unsigned int _arrayLength, float _deltaTime);
	void UpdateElements(EntityID* _arrayOfEntityIds, unsigned int _arrayLength, float _deltaTime);

	void* CreateElement(unsigned int _index);
	void* CreateElement(EntityID _entityId);
	void CreateElements(unsigned int* _arrayOfIndex, unsigned int _arrayLength);
	void CreateElements(EntityID* _arrayOfEntityIds, unsigned int _arrayLength);

	void* CreateElementWithCopyConstructor(unsigned int _index, const void* _elementToCopy);
	void* CreateElementWithCopyConstructor(EntityID _entityId, const void* _elementToCopy);
	void CreateElementsWithCopyConstructor(unsigned int* _arrayOfIndex, unsigned int _arrayLength, const void* _elementToCopy);
	void CreateElementsWithCopyConstructor(EntityID* _arrayOfEntityIds, unsigned int _arrayLength, const void* _elementToCopy);

	void CallElementDestructor(unsigned int _index);
	void CallElementDestructor(EntityID _entityId);
	void CallElementsDestructor(unsigned int* _arrayOfIndex, unsigned int _arrayLength);
	void CallElementsDestructor(EntityID* _arrayOfEntityIds, unsigned int _arrayLength);

	void CallStoredFunctionWithObjectParam(unsigned int _index, void* _object);
	void CallStoredFunctionWithObjectParam(EntityID _entityId, void* _object);
	void CallStoredFunctionWithObjectParam(unsigned int* _arrayOfIndex, unsigned int _arrayLength, void* _object);
	void CallStoredFunctionWithObjectParam(EntityID* _arrayOfEntityIds, unsigned int _arrayLength, void* _object);

	bool SerializeElement(unsigned int _index, pugi::xml_node* _ComponentNode);
	bool SerializeElement(EntityID _entityId, pugi::xml_node* _ComponentNode);

	bool LoadElement(unsigned int _index, pugi::xml_node* _ComponentNode);
	bool LoadElement(EntityID _entityId, pugi::xml_node* _ComponentNode);
};