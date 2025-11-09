// 
// PABLO GRANADO VALDIVIELSO
// 
// Main.cpp : Defines the entry point for the application.
//

#include "Engine/Engine.h"
#include "Game/PoolInitializationClass.h"
#include "Game/Level.h"
#include "Game/GameScoreCounter.h"

int main()
{
	Engine engine;
	PoolInitializationClass poolInitClass;

	if (engine.Init(&poolInitClass));
	{
		// Getting the first deltaTime out of the way, which will be equal to 0.02f.
		engine.UpdateDeltaTime();

		// Loading the level from an XML file.
		Level("Assets/Levels/MainLevel.xml").Instantiate();

		// Game Loop
		while (engine.ShouldClose())
		{
			engine.UpdateDeltaTime();

			// Updating the physics.
			engine.UpdatePhysics();

			// Updating the components.
			engine.UpdateLogic();

			// Rendering our entities.
			engine.ClearScreen();
			engine.GetPoolManager()->RenderEntities();

			// Rendering the debug text for the Score Counter. This is necessary cause the ECS was not build to allow for multiple render passes.
			engine.GetPoolManager()->GetEntityPool(0)->GetComponent<GameScoreCounter>(engine.GetPoolManager()->GetEntityPool(0)->m_entities[0].m_id)->RenderDebugText();

			engine.UpdateTigrScreen();

			// Limiting the Frame Rate to the desired ammount.
			if (engine.CalculateUnscaledDeltaTimeSinceBeginningOfFrame() < 1 / Engine::FPS_Target)
			{
				engine.Wait(1 / Engine::FPS_Target - engine.CalculateUnscaledDeltaTimeSinceBeginningOfFrame());
			}
		}

		engine.Quit();
	}
}