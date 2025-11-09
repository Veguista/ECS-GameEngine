#include "Game/Level.h"
#include "Engine/Util/XML/XML_File_Handler.h"
#include "Engine/ECS/ECS_PoolManager.h"
#include "Engine/DataTypes/Prefabs/Prefab.h"

Level::Level(const std::string& _LevelPath)
	: LevelPath{ _LevelPath }
{
	pugi::xml_parse_result result = XML_UTIL::ReadXMLFile(LevelPath, LevelInfo);

	if (result)
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
	else
	{
		printf("Prefab couldn't be loaded at location %s. %s.\n", LevelPath.c_str(), result.description());
	}
}

Level::Level(const char* _LevelPath)
	: LevelPath{ _LevelPath }
{
	pugi::xml_parse_result result = XML_UTIL::ReadXMLFile(LevelPath, LevelInfo);

	if (result)
	{
		SuccessfullyLoaded = true;
		LastChangesSaved = true;
	}
	else
	{
		printf("Prefab couldn't be loaded at location %s. %s.\n", LevelPath.c_str(), result.description());
	}
}

bool Level::SaveLevelToFile(std::vector<Prefab*>& _listOfPrefabsToSave)
{
	LastChangesSaved = false;

	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	if (poolManager == nullptr)
	{
		return false;
	}

	pugi::xml_node parentNode = LevelInfo.child("Level").child("PrefabList");
	if (parentNode.empty())
	{
		parentNode.remove_children();
		parentNode.remove_attributes();

		// If we can't find the "Level" child, we create it.
		LevelInfo.reset();
		parentNode = LevelInfo.append_child("Level").append_child("PrefabList");
		
		// If even after creating it, we can't find it, we desist.
		if (parentNode.empty())
		{
			return false;
		}
	}
	
	// we reset the Parent Node, in case we need to override previous changes.
	parentNode.remove_children();
	parentNode.remove_attributes();

	for (int prefabIndex = 0; prefabIndex < _listOfPrefabsToSave.size(); prefabIndex++)
	{
		if (!_listOfPrefabsToSave[prefabIndex]->HaveLastChangesBeenSaved())
		{
			// If it isn't saved, we try to save it.
			if (!_listOfPrefabsToSave[prefabIndex]->SaveToFile())
			{
				// If we failed the save, we do not serialize this Prefab.
				continue;
			}
		}

		Prefab* prefab = _listOfPrefabsToSave[prefabIndex];

		pugi::xml_node prefabNode = parentNode.append_child("Prefab");
		if (prefabNode.empty())
		{
			continue;
		}

		prefabNode.append_attribute("Path").set_value(prefab->GetPrefabPath());
	}

	return SaveToFile();
}

bool Level::SaveToFile()
{
	if (LevelPath.empty())
	{
		return false;
	}

	std::string filePath = std::string(LevelPath);

	if (XML_UTIL::SaveXMLFile(filePath, LevelInfo))
	{
		LastChangesSaved = true;
		return true;
	}

	return false;
}

bool Level::Instantiate()
{
	ECS_PoolManager* poolManager = ECS_PoolManager::GetInstance();
	if (!SuccessfullyLoaded || poolManager == nullptr)
	{
		return false;
	}

	pugi::xml_node parentNode = LevelInfo.child("Level").child("PrefabList");

	if (parentNode.empty())
	{
		return false;
	}

	for (auto prefabNode : parentNode.children("Prefab"))
	{
		const pugi::char_t* prefabPath = prefabNode.attribute("Path").value();
		Prefab prefab = Prefab(prefabPath);

		prefab.Instantiate();
	}

	return true;
}
