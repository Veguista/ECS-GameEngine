#include "Prefab.h"
#include "Engine/Engine.h"
#include "Engine/Util/XML/XML_File_Handler.h"
#include "Engine/ECS/ECS_PoolManager.h"
#include "Engine/ECS/ECS_Configuration.h"


Prefab::Prefab(const EntityID& _entityID, const std::string& _PrefabPath, const std::string& _PrefabName)
	: PrefabPath {_PrefabPath }
{
	pugi::xml_node parentNode = PrefabInfo.append_child("Prefab");

	// Setting the prefab name.
	if (PrefabPath.back() != '/')
	{
		PrefabPath.push_back('/');
	}

	PrefabPath.append(_PrefabName);

	PrefabPath.append(Engine::XML_FILE_EXTENSION);
	parentNode.append_attribute("Name").set_value(PrefabPath.c_str());

	if (SaveEntityToFile(_entityID))
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
}

Prefab::Prefab(const EntityID& _entityID, const char* _PrefabPath, const char* _PrefabName)
	: PrefabPath{ _PrefabPath }
{
	pugi::xml_node parentNode = PrefabInfo.append_child("Prefab");

	// Setting the prefab name.
	if (PrefabPath.back() != '/')
	{
		PrefabPath.push_back('/');
	}

	PrefabPath.append(_PrefabName);
	PrefabPath.append(Engine::XML_FILE_EXTENSION);
	parentNode.append_attribute("Name").set_value(PrefabPath.c_str());

	if (SaveEntityToFile(_entityID))
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
}

Prefab::Prefab(const std::string& _PrefabPath, const std::string& _PrefabName)
	: PrefabPath{ _PrefabPath }
{
	// Setting the prefab name.
	if (PrefabPath.back() != '/')
	{
		PrefabPath.push_back('/');
	}

	PrefabPath.append(_PrefabName);
	PrefabPath.append(Engine::XML_FILE_EXTENSION);

	if (XML_UTIL::ReadXMLFile(PrefabPath, PrefabInfo))
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
}

Prefab::Prefab(const char* _PrefabPath, const char* _PrefabName)
	: PrefabPath{ _PrefabPath }
{
	// Setting the prefab name.
	if (PrefabPath.back() != '/')
	{
		PrefabPath.push_back('/');
	}

	PrefabPath.append(_PrefabName);
	PrefabPath.append(Engine::XML_FILE_EXTENSION);

	pugi::xml_parse_result result = XML_UTIL::ReadXMLFile(PrefabPath, PrefabInfo);

	if (result)
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
	else
	{
		printf("Prefab couldn't be loaded at location %s. %s.\n", (std::string(_PrefabPath) + std::string(_PrefabName)).c_str(), result.description());
	}
}

Prefab::Prefab(const std::string& _PrefabFilePath)
	: PrefabPath { _PrefabFilePath }
{
	pugi::xml_parse_result result = XML_UTIL::ReadXMLFile(_PrefabFilePath, PrefabInfo);

	if (result)
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
	else
	{
		printf("Prefab couldn't be loaded at location %s. %s.\n", _PrefabFilePath.c_str(), result.description());
	}
}

Prefab::Prefab(const char* _PrefabFilePath)
	: PrefabPath{ std::string(_PrefabFilePath) }
{
	pugi::xml_parse_result result = XML_UTIL::ReadXMLFile(_PrefabFilePath, PrefabInfo);

	if (result)
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
	else
	{
		printf("Prefab couldn't be loaded at location %s. %s.\n", _PrefabFilePath, result.description());
	}
}

bool Prefab::SaveEntityToFile(const EntityID& _entityID)
{
	LastChangesSaved = false;

	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	if (poolManager == nullptr)
	{
		return false;
	}

	// Setting the entityPool name.
	pugi::xml_node poolNode = poolManager->GetPoolListNode().find_child_by_attribute("PoolID", std::to_string(ECS::GetPoolFromId(_entityID)).c_str());
	pugi::xml_node parentNode = PrefabInfo.child("Prefab");
	
	if (poolNode.empty() || parentNode.empty())
	{
		return false;
	}

	// we reset the Parent Node, in case we need to override previous changes.
	parentNode.remove_children();
	parentNode.remove_attributes();
	parentNode.append_attribute("PoolName").set_value(poolNode.attribute("PoolName").value());

	// Saving the Components of this entity.
	ECS_EntityPool* entityPool = poolManager->GetEntityPool(ECS::GetPoolFromId(_entityID));
	if (entityPool == nullptr)
	{
		return false;
	}

	EntityComponentMask entityComponentMask = entityPool->m_entities[ECS::GetIndexFromId(_entityID)].m_componentMask;
	pugi::xml_node componentsNode = parentNode.append_child("ListOfComponents");

	for (ComponentIndex compIndex = 0; compIndex < MAX_NUMBER_OF_COMPONENTS_INSIDE_ENTITY_POOL; compIndex++)
	{
		if (entityComponentMask.test(compIndex))
		{
			pugi::xml_node componentInPoolList = poolNode.child("Components").find_child_by_attribute("ComponentIndex", std::to_string(compIndex).c_str());

			if (componentInPoolList.empty())
			{
				continue;
			}

			std::string componentName = componentInPoolList.attribute("ComponentName").value();
			pugi::xml_node component = componentsNode.append_child("Component");
			component.append_attribute("ComponentName").set_value(componentName.c_str());

			// If our component can be serialized, we do it.
			if (poolManager->IsComponentSerializable(compIndex))
			{
				entityPool->m_componentPools[compIndex]->SerializeElement(_entityID, &component);
			}
		}
	}

	return SaveToFile();
}

bool Prefab::SaveToFile()
{
	if (PrefabPath.empty())
	{
		return false;
	}

	if (XML_UTIL::SaveXMLFile(PrefabPath, PrefabInfo))
	{
		LastChangesSaved = true;
		return true;
	}

	return false;
}

EntityID Prefab::Instantiate() const
{
	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	if (!SuccessfullyLoaded || poolManager == nullptr)
	{
		return false;
	}

	pugi::xml_node parentNode = PrefabInfo.child("Prefab");
	pugi::xml_node componentsNode = parentNode.child("ListOfComponents");
	pugi::xml_node poolNode = poolManager->GetPoolListNode().find_child_by_attribute("PoolName", parentNode.attribute("PoolName").value());
	pugi::xml_node poolComponentsNode = poolNode.child("Components");

	if (parentNode.empty() || componentsNode.empty() || poolNode.empty() || poolComponentsNode.empty())
	{
		return ECS::CONSTANTS::InvalidEntityID();
	}

	ECS_EntityPool* entityPool = poolManager->GetEntityPool(static_cast<PoolID>(std::stof(poolNode.attribute("PoolID").value())));
	if (entityPool == nullptr)
	{
		return ECS::CONSTANTS::InvalidEntityID();
	}

	EntityID entityID = entityPool->CreateEntity();

	for (auto componentNode : componentsNode.children("Component"))
	{
		pugi::xml_node componentInPoolDoc = poolComponentsNode.find_child_by_attribute("ComponentName", componentNode.attribute("ComponentName").value());
		auto componentIndexStr = componentInPoolDoc.attribute("ComponentIndex").value();
		ComponentIndex componentIndex = static_cast<ComponentIndex>(std::stof(componentIndexStr));
	
		void* instantiatedComponent = entityPool->AssignComponent(ECS::GetIndexFromId(entityID), componentIndex);

		if (instantiatedComponent != nullptr && !componentNode.children().empty())
		{
			entityPool->m_componentPools[componentIndex]->LoadElement(entityID, &componentNode);
		}
	}

	return entityID;
}
