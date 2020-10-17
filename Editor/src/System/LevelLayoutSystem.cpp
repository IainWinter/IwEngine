#include "Systems/LevelLayoutSystem.h"

#include "iw/reflection/serialization/JsonSerializer.h"

#include "iw/reflection/reflect/std/vector.h"
#include "iw/reflection/reflect/std/string.h"
#include "iw/reflect/Components/LevelLayout.h"

LevelLayoutSystem::LevelLayoutSystem()
	: iw::SystemBase("Level Layout")
	, m_currentWorld(nullptr)
{}

int LevelLayoutSystem::Initialize() {
	std::string startingLevel = "levels/canyon/canyon01.json";

	LevelLayout* canyon01 = new LevelLayout(); // keep all alive
	LevelLayout* cave01   = new LevelLayout();

	LevelLayout /***^***/ canyon02, canyon03,  canyon04, canyon04a, canyon05,
	            canyon06, canyon07, canyon07a, canyon08, canyon09,  canyon10,
	            /**^**/ cave02, cave03, cave03a, cave04,
	            cave05, cave06, cave07, cave08,
	            top01, top02, top03, top04,
	            top05, top06, top07, top08;

	canyon01->LevelName = "levels/canyon/canyon01.json";
	canyon02 .LevelName = "levels/canyon/canyon02.json";
	canyon03 .LevelName = "levels/canyon/canyon03.json";
	canyon04 .LevelName = "levels/canyon/canyon04.json";
	canyon04a.LevelName = "levels/canyon/canyon04.a.json";
	canyon05 .LevelName = "levels/canyon/canyon05.json";
	canyon06 .LevelName = "levels/canyon/canyon06.json";
	canyon07 .LevelName = "levels/canyon/canyon07.json";
	canyon07a.LevelName = "levels/canyon/canyon07.a.json";
	canyon08 .LevelName = "levels/canyon/canyon08.json";
	canyon09 .LevelName = "levels/canyon/canyon09.json";
	canyon10 .LevelName = "levels/canyon/canyon11.json";

	cave01->LevelName = "levels/canyon/cave01.json";
	cave02 .LevelName = "levels/canyon/cave02.json";
	cave03 .LevelName = "levels/canyon/cave03.json";
	cave03a.LevelName = "levels/canyon/cave03.a.json";
	cave04 .LevelName = "levels/canyon/cave04.json";
	cave05 .LevelName = "levels/canyon/cave05.json";
	cave06 .LevelName = "levels/canyon/cave06.json";
	cave07 .LevelName = "levels/canyon/cave07.json";
	cave08 .LevelName = "levels/canyon/cave08.json";

	top01.LevelName = "levels/canyon/top01.json";
	top02.LevelName = "levels/canyon/top02.json"; 
	top03.LevelName = "levels/canyon/top03.json"; 
	top04.LevelName = "levels/canyon/top04.json";
	top05.LevelName = "levels/canyon/top05.json";
	top06.LevelName = "levels/canyon/top06.json";
	top07.LevelName = "levels/canyon/top07.json";
	top08.LevelName = "levels/canyon/top08.json";

	top06.AddConnection(top07);
	top05.AddConnection(top06);
	top04.AddConnection(top05);
	top03.AddConnection(top04);
	top02.AddConnection(top03);
	top01.AddConnection(top02);

	cave08 .AddConnection(top01);
	cave07 .AddConnection(cave08);
	cave06 .AddConnection(cave07);
	cave05 .AddConnection(cave06);
	cave04 .AddConnection(cave05);
	cave03 .AddConnection(cave04);
	cave03 .AddConnection(cave03a);
	cave02 .AddConnection(cave03);
	cave01->AddConnection(cave02);

	canyon09 .AddConnection(canyon10);
	canyon08 .AddConnection(canyon09);
	canyon07 .AddConnection(canyon08);
	canyon07 .AddConnection(canyon07a);
	canyon06 .AddConnection(canyon07);
	canyon05 .AddConnection(canyon06);
	canyon04 .AddConnection(canyon05);
	canyon04 .AddConnection(canyon04a);
	canyon03 .AddConnection(canyon04);
	canyon02 .AddConnection(canyon03);
	canyon01->AddConnection(canyon02);

	iw::JsonSerializer("C:/dev/IwEngine/Editor/assets/levels/canyon.json").Write(*canyon01);
	iw::JsonSerializer("C:/dev/IwEngine/Editor/assets/levels/cavTop.json").Write(*cave01);

	WorldLayout* canyon = new WorldLayout(canyon01);
	WorldLayout* cavTop = new WorldLayout(cave01);

	m_worlds.push_back(canyon);
	m_worlds.push_back(cavTop);

	Bus->push<GotoLevelEvent>(startingLevel);

	return 0;
}

bool LevelLayoutSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::RESET_LEVEL): {
			Bus->push<DeactivateLevelEvent>(m_currentWorld->CurrentLevel->LevelName);
			Bus->push<ActivateLevelEvent>  (m_currentWorld->CurrentLevel->LevelName, 0);

			break;
		}
		case iw::val(Actions::GOTO_LEVEL): {
			GotoLevelEvent& event = e.as<GotoLevelEvent>();

			FillWorlds(event.LevelName);

			Bus->push<UnloadLevelEvent>("All");

			Bus->push<LoadLevelEvent>(m_currentWorld->CurrentLevel->LevelName, "");
			for (LevelLayout& connection : m_currentWorld->CurrentLevel->Connections) {
				Bus->push<LoadLevelEvent>(connection.LevelName, m_currentWorld->CurrentLevel->LevelName);
			}

			Bus->push<ActivateLevelEvent>(m_currentWorld->CurrentLevel->LevelName, 101);

			break;
		}
		case iw::val(Actions::GOTO_CONNECTED_LEVEL): {
			GotoConnectedLevelEvent& event = e.as<GotoConnectedLevelEvent>();

			Bus->push<DeactivateLevelEvent>(m_currentWorld->CurrentLevel->LevelName);

			// -1 to previous level, activate now 'current' level
			if (event.Index < 0) {
				m_currentWorld->ToPreviousLevel();
			}

			else {
				if (   event.Index == 1
					&& m_currentWorld->PreviousLevels.size() > 1) // index = 1 means that we are moving to next full level
				{
					for (unsigned i = 1; i < m_currentWorld->CurrentLevel->Connections.size(); i++) {
						Bus->push<UnloadLevelEvent>(m_currentWorld->CurrentLevel->Connections.at(i).LevelName);
					}
					Bus->push<UnloadLevelEvent>(m_currentWorld->PreviousLevels.top()->LevelName);
				}

				if (event.Index > 0) { // if the index is > 0 then we arn't resetting the level & need to move
					m_currentWorld->ToNextLevel(event.Index - 1);
				}

				else {
					Bus->push<LoadLevelEvent>(m_currentWorld->CurrentLevel->LevelName, "");
				}

				// Load all connections to new current level

				for (LevelLayout& connection : m_currentWorld->CurrentLevel->Connections) {
					Bus->push<LoadLevelEvent>(connection.LevelName, m_currentWorld->CurrentLevel->LevelName);
				}
			}

			Bus->push<ActivateLevelEvent>(m_currentWorld->CurrentLevel->LevelName, event.Index);

			break;
		}
	}

	return false;
}

void LevelLayoutSystem::FillWorlds(
	const std::string& untilLevel)
{
	for (WorldLayout* world : m_worlds) {
		world->FillWorld(untilLevel);
		if (world->CurrentLevel) {
			m_currentWorld = world;
			break;
		}
	}
}

// World Layout

void LevelLayoutSystem::WorldLayout::ToNextLevel(
	unsigned index)
{
	PreviousLevels.push(CurrentLevel);
	CurrentLevel = CurrentLevel->Next(index);
}

void LevelLayoutSystem::WorldLayout::ToPreviousLevel() {
	CurrentLevel = PreviousLevels.top();
	PreviousLevels.pop();
}

void LevelLayoutSystem::WorldLayout::FillWorld(
	const std::string& untilLevel)
{
	UnwindWorld();

	while (CurrentLevel && CurrentLevel->LevelName != untilLevel) {
		int next = 0;

		for (unsigned i = 0; i < CurrentLevel->Connections.size(); i++) {
			if (CurrentLevel->Connections.at(i).LevelName == untilLevel) {
				next = i;
			}
		}

		ToNextLevel(next);
	}
}

void LevelLayoutSystem::WorldLayout::UnwindWorld() {
	while (PreviousLevels.size() > 0) {
		ToPreviousLevel();
	}
}
