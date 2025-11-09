#pragma once

#include "ECS_Typedefs.h"
#include "ECS_Constants.h"

namespace ECS
{
#pragma region OLD WAY OF CREATING ENTITY ID

	//static consteval unsigned int InvalidEntityIndex() { return static_cast<unsigned int>(-1); }
	//static consteval unsigned int InvalidEntityVersion() { return static_cast<unsigned int>(-1); }
	//
	//static EntityID CreateEntityId(unsigned int _index, unsigned int _version)
	//{
	//	return static_cast<EntityID>(_index) << 32 | (static_cast<EntityID>(_version));
	//}
	//
	//static unsigned int GetIndexFromId(EntityID _id)
	//{
	//	return _id >> 32;
	//}
	//static unsigned int GetVersionFromId(EntityID _id)
	//{
	//	return static_cast<unsigned int>(_id);
	//}
	//
	//static bool IsIndexValid(EntityID _id)
	//{
	//	return GetIndexFromId(_id) != InvalidEntityIndex();
	//}
	//static bool IsIndexValid(unsigned int _index)
	//{
	//	return _index != InvalidEntityIndex();
	//}

#pragma endregion

	// The disposition of an EntityId is: 24 bits to Index - 12 bits to PoolId - 28 bits to Version. 

	static constexpr EntityID CreateEntityId(unsigned int _index, unsigned int _poolId, unsigned int _version)
	{
		return static_cast<EntityID>(_index) << 40 | static_cast<EntityID>(_poolId) << 28 | (static_cast<EntityID>(_version));
	}

	static unsigned int GetIndexFromId(EntityID _id)
	{
		return _id >> 40;
	}
	static PoolID GetPoolFromId(EntityID _id)
	{
		return static_cast<PoolID>(_id >> 28) & ECS::CONSTANTS::InvalidPoolId();
	}
	static unsigned int GetVersionFromId(EntityID _id)
	{
		return static_cast<unsigned int>(_id) & ECS::CONSTANTS::InvalidEntityVersion();
	}

	static bool IsIndexValid(EntityID _id)
	{
		return GetIndexFromId(_id) < ECS::CONSTANTS::InvalidEntityIndex();
	}
	static bool IsIndexValid(unsigned int _index)
	{
		return _index < ECS::CONSTANTS::InvalidEntityIndex();
	}
}