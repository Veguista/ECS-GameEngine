#pragma once

#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include <string>
#include <vector>

class Prefab;

struct Level
{
	std::string LevelPath;
	pugi::xml_document LevelInfo;
	bool LastChangesSaved = false;
	bool SuccessfullyLoaded = false;

	bool SaveToFile();

public:
	Level(const std::string& _LevelPath);
	Level(const char* _LevelPath);

	bool SaveLevelToFile(std::vector<Prefab*>& _listOfEntityIDsToSave);
	bool Instantiate();
};