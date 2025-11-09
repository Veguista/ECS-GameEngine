#pragma once

#include "Engine/ECS/ECS_Constants.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include <vector>
#include <string>


class Prefab
{
	std::string PrefabPath;
	pugi::xml_document PrefabInfo;
	bool LastChangesSaved = false;
	bool SuccessfullyLoaded = false;

public:
	Prefab(const EntityID& _entityID, const std::string& _PrefabPath, const std::string& _PrefabName);
	Prefab(const EntityID& _entityID, const char* _PrefabPath, const char* _PrefabName);
	Prefab(const std::string& _PrefabPath, const std::string& _PrefabName);
	Prefab(const char* _PrefabPath, const char* _PrefabName);
	Prefab(const std::string& _PrefabFilePath);
	Prefab(const char* _PrefabFilePath);

	inline const std::string& GetPrefabPath() const { return PrefabPath; };
	inline bool HaveLastChangesBeenSaved() const { return LastChangesSaved; };
	inline bool IsSuccessfullyLoaded() const { return SuccessfullyLoaded; };

	bool SaveEntityToFile(const EntityID& _entityID);
	bool SaveToFile();
	EntityID Instantiate() const;
};