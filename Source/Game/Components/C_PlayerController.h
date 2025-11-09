#pragma once

#include "Engine/ECS/ECS_Interfaces.h"
#include "Engine/ECS/ECS_EntityID.h"

struct Tigr;

struct C_PlayerController : IECS_Update
{
	C_PlayerController();
	~C_PlayerController();

private:
	const float PLAYER_SPEED = 100.0f;

	EntityID OwnerEntityId = ECS::CONSTANTS::InvalidEntityID();
	Tigr* window = nullptr;
	static inline C_PlayerController* Instance = nullptr;

	int RawHorizontalInput = 0;
	int RawVerticalInput = 0;
	bool RunStarted = false;

	void MovePlayer(float _DeltaTime);

public:
	inline EntityID GetPlayerEntityID() const { return OwnerEntityId; };
	void Update(float _DeltaTime);
	void TryToRestartGame();
	void TryToEndGame();
	static inline C_PlayerController* GetInstance() { return Instance; };
};