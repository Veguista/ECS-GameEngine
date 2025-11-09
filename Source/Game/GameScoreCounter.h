#pragma once

#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/ExternalLibraries/Pugixml/pugixml.hpp"
#include <string>

struct GameScoreCounter : IECS_Update, IECS_Serializable
{
private:
	const std::string ScoresFilePath = std::string("Assets/SaveFiles/Scores.xml");
	pugi::xml_document ScoresInfo;
	bool RunStarted;

	float BestScore = 0;
	float CurrentScore = 0;

public:
	void StartNewRun();
	void EndRun();

	void Update(float _DeltaTime);
	void RenderDebugText();

	bool Serialize(pugi::xml_node* _ComponentNode);
	bool Load(const pugi::xml_node* _ComponentNode);
};