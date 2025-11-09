#pragma once

#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/DataTypes/Prefabs/Prefab.h"
#include <string>

struct BubbleSpawner : IECS_Update
{
	const std::string BlueBubblePrefabPath = std::string("Assets/Prefabs/BlueBubblePrefab.xml");
	const std::string GreenBubblePrefabPath = std::string("Assets/Prefabs/GreenBubblePrefab.xml");
	const std::string RedBubblePrefabPath = std::string("Assets/Prefabs/RedBubblePrefab.xml");

	const Prefab BlueBubblePrefab = Prefab(BlueBubblePrefabPath);
	const Prefab GreenBubblePrefab = Prefab(GreenBubblePrefabPath);
	const Prefab RedBubblePrefab = Prefab(RedBubblePrefabPath);

	float BubbleSpawnTimer = 0.0f;
	const float MaxTimer = 5.0f;

	void Update(float _DeltaTime);
};