#pragma once

#include "ECS_Typedefs.h"
#include "ECS_EntityID.h"

struct ECS_Entity
{
	EntityComponentMask m_componentMask;
	EntityID m_id;

	// Constructor
	ECS_Entity(const EntityID& _id, const EntityComponentMask& _componentMask) : m_id{ _id }, m_componentMask{ _componentMask } {};
	ECS_Entity(const ECS_Entity& _other) : m_componentMask{ _other.m_componentMask }, m_id{ _other.m_id } {};

	inline EntityID SetIndex(const unsigned int _newIndex) 
		{ return m_id = ECS::CreateEntityId(_newIndex, ECS::GetPoolFromId(m_id), ECS::GetVersionFromId(m_id)); };
};