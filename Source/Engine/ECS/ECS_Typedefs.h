#pragma once

#include "ECS_Configuration.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include <bitset>

typedef unsigned long long EntityID;
typedef short unsigned int PoolID;
typedef unsigned char ComponentIndex;
typedef std::bitset<MAX_TOTAL_NUMBER_OF_COMPONENTS> PoolComponentMask;
typedef std::bitset<MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL> EntityComponentMask;

typedef void (*delayed_updater_func)(void*, float);
typedef void (*delayed_destructor_func)(void*);
typedef void* (*delayed_constructor_func)(void*);
typedef void* (*delayed_copy_constructor_func)(void*, const void*);
typedef void (*delayed_funct_plus_one_object_param)(void*, void*); // Don't be fooled by the "one_object_param", we still need an additional pointer to the object where the function is called.
typedef bool (*delayed_funct_serialize)(void*, pugi::xml_node*);
