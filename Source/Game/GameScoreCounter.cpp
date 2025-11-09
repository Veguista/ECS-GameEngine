#include "GameScoreCounter.h"
#include "Engine/Util/XML/XML_File_Handler.h"
#include "Engine/ExternalLibraries/Tigr/tigr.h"
#include "Engine/Engine.h"

void GameScoreCounter::StartNewRun()
{
	RunStarted = true;
}

void GameScoreCounter::EndRun()
{
	CurrentScore = 0;
	RunStarted = false;
	Serialize(nullptr); // It doesn't matter what node we pass on here, cause we won't be using in this specific implementation.
}

void GameScoreCounter::Update(float _DeltaTime)
{
	if (RunStarted)
	{
		// Updating the current Score.
		CurrentScore += _DeltaTime;

		if (CurrentScore > BestScore)
		{
			BestScore = CurrentScore;
		}
	}
}

void GameScoreCounter::RenderDebugText()
{
	if(RunStarted)
	{
		// Showcasing the current Score.
		tigrPrint(Engine::GetInstance()->GetTigrScreen(), tfont, 300, 20, tigrRGBA(0xff, 0xff, 0xff, 0xff), "Current Score : %f", CurrentScore);
	}
	else
	{
		// We showcase a prompt to start the game with Space.
		tigrPrint(Engine::GetInstance()->GetTigrScreen(), tfont, 300, 20, tigrRGBA(0xff, 0xff, 0xff, 0xff), "Press SPACE to restart.");
	}

	tigrPrint(Engine::GetInstance()->GetTigrScreen(), tfont, 15, 20, tigrRGBA(0xff, 0xff, 0xff, 0xff), "Best Score : %f", BestScore);
}

bool GameScoreCounter::Serialize(pugi::xml_node* _ComponentNode)
{
	if (_ComponentNode != nullptr)
	{
		_ComponentNode->append_child("Scores"); // We need to append a bogus child so that the system recognizes that there is something to load later down the line.
	}

	pugi::xml_node scoresNodes = ScoresInfo.child("Scores");

	scoresNodes.remove_children();
	scoresNodes.remove_attributes();

	pugi::xml_node highestScoreNode = scoresNodes.append_child("HighestScore");
	XML_UTIL::SaveToXMLNode(BestScore, highestScoreNode);

	if (ScoresFilePath.empty())
	{
		return false;
	}

	return XML_UTIL::SaveXMLFile(ScoresFilePath, ScoresInfo);
}

bool GameScoreCounter::Load(const pugi::xml_node* _ComponentNode)
{
	// If we failed to load our scores file.
	if (!XML_UTIL::ReadXMLFile(ScoresFilePath, ScoresInfo))
	{
		// We create the root for our info document.
		ScoresInfo.append_child("Scores");

		// We reset our scores.
		BestScore = 0;
	}
	else // Else, we load it.
	{
		pugi::xml_node highestScoreNode = ScoresInfo.child("Scores").child("HighestScore");
		if (!highestScoreNode.empty())
		{
			XML_UTIL::LoadXMLNodeToVariable(BestScore, highestScoreNode);
		}
	}

	return true;
}
