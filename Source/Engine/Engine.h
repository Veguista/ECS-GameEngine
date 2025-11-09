#pragma once

#include "Engine/EngineConfiguration.h"
#include "Engine/ECS/ECS_PoolManager.h"
#include <string>
#include <sstream>

struct Tigr;
class ECS_Pool_Initializer;

class Engine
{
	// CONSTANTS
public:
	static constexpr float FPS_Target{ CONFIG_FPS_TARGET };
#undef CONFIG_FPS_TARGET

	static inline const std::string CONFIGURATION_FILES_PATH{ "Assets/EngineConfigFiles/" };
	static inline const std::string POOL_FILE_NAME{ "ECS_Pools_Information" };
	static inline const std::string XML_FILE_EXTENSION{ ".xml" };

private:
	static inline Engine* Instance{ nullptr };

	bool m_isRunning{ false };

	float m_timeScale{ 1 };
	float m_additionalTimeSpentAfterFrame{ 0 };

	float deltaTime{ 0 };
	float unscaledDeltaTime{ 0 };

	Tigr* m_pScreen { nullptr };
	ECS_PoolManager* m_pPoolManager { nullptr };

	// Functions

#pragma region ENGINE CONTROLS
public:
	bool Init(ECS_Pool_Initializer* _PoolInitializerClass);
	bool Input();
	bool UpdateTigrScreen();
	bool UpdateDeltaTime();
	float CalculateUnscaledDeltaTimeSinceBeginningOfFrame();
	void UpdatePhysics();
	bool UpdateLogic();
	bool ClearScreen();
	bool Quit();

	static inline const Engine* GetInstance() {	return Instance; };

#pragma endregion

#pragma region ENGINE FUNCTIONS
public:
	inline ECS_PoolManager* GetPoolManager() const { return m_pPoolManager; };
	inline Tigr* GetTigrScreen() const { return m_pScreen; };
	inline float GetDeltaTime() const { return deltaTime; };

	bool ShouldClose();
	inline void LogMe(const std::string& _text) const { printf("%s\n", _text); };
	inline void LogMe(const std::string&& _text) const { printf("%s\n", _text); };
	void Print(const char* text);
	void Wait(float _secs);

	template<typename T>
	std::string ToString(const T& _text)
	{
		std::stringstream ss;
		ss << _text;
		return ss.str();
	}

#pragma endregion

};
