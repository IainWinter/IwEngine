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
	std::string startingLevel = "levels/canyon/cave07.json";

	LevelLayout* forest01 = new LevelLayout();

	LevelLayout   forest02, forest03, forest04, forest05, forest05a, forest06, forest07, forest07a,
		forest08, forest09, forest10, forest11, forest12, forest12a, forest13,  forest14,
		forest15, forest16, forest17, forest18, forest19, forest20,  forest21, forest22, forest23;

	forest01->LevelName = "levels/forest/forest01.json";
	forest02 .LevelName = "levels/forest/forest02.json";
	forest03 .LevelName = "levels/forest/forest03.json";
	forest04 .LevelName = "levels/forest/forest04.json";
	forest05 .LevelName = "levels/forest/forest05.json";
	forest05a.LevelName = "levels/forest/forest05.a.json";
	forest06 .LevelName = "levels/forest/forest06.json";
	forest07 .LevelName = "levels/forest/forest07.json";
	forest07a.LevelName = "levels/forest/forest07.a.json";
	forest08 .LevelName = "levels/forest/forest08.json";
	forest09 .LevelName = "levels/forest/forest09.json";
	forest10 .LevelName = "levels/forest/forest10.json";
	forest11 .LevelName = "levels/forest/forest11.json";
	forest12 .LevelName = "levels/forest/forest12.json";
	forest12a.LevelName = "levels/forest/forest12a.json";
	forest13 .LevelName = "levels/forest/forest13.json";
	forest14 .LevelName = "levels/forest/forest14.json";
	forest15 .LevelName = "levels/forest/forest15.json";
	forest16 .LevelName = "levels/forest/forest16.json";
	forest17 .LevelName = "levels/forest/forest17.json";
	forest18 .LevelName = "levels/forest/forest18.json";
	forest19 .LevelName = "levels/forest/forest19.json";
	forest20 .LevelName = "levels/forest/forest20.json";
	forest21 .LevelName = "levels/forest/forest21.json";
	forest22 .LevelName = "levels/forest/forest22.json";
	forest23 .LevelName = "levels/forest/forest23.json";

	forest22 .AddConnection(forest23);
	forest21 .AddConnection(forest22);
	forest20 .AddConnection(forest21);
	forest19 .AddConnection(forest20);
	forest18 .AddConnection(forest19);
	forest17 .AddConnection(forest18);
	forest16 .AddConnection(forest17);
	forest15 .AddConnection(forest16);
	forest14 .AddConnection(forest15);
	forest13 .AddConnection(forest14);
	forest12 .AddConnection(forest13);
	forest12 .AddConnection(forest12a);
	forest11 .AddConnection(forest12);
	forest10 .AddConnection(forest11);
	forest09 .AddConnection(forest10);
	forest08 .AddConnection(forest09);
	forest07 .AddConnection(forest08);
	forest07 .AddConnection(forest07a);
	forest06 .AddConnection(forest07);
	forest05 .AddConnection(forest06);
	forest05 .AddConnection(forest05a);
	forest04 .AddConnection(forest05);
	forest03 .AddConnection(forest04);
	forest02 .AddConnection(forest03);
	forest01->AddConnection(forest02);

	LevelLayout* canyon01 = new LevelLayout(); // keep all alive
	LevelLayout* cave01   = new LevelLayout();
	LevelLayout* top01    = new LevelLayout();

	LevelLayout /***^***/ canyon02, canyon03,  canyon04, canyon04a, canyon05,
	            canyon06, canyon07, canyon07a, canyon08, canyon09,  canyon10, canyon11,
	            /**^**/ cave02, cave03, cave03a, cave04,
	            cave05, cave06, cave07, cave08,
	            /****/ top02, top03, top04,
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
	canyon10 .LevelName = "levels/canyon/canyon10.json";
	canyon11 .LevelName = "levels/canyon/canyon11.json";

	cave01->LevelName = "levels/canyon/cave01.json";
	cave02 .LevelName = "levels/canyon/cave02.json";
	cave03 .LevelName = "levels/canyon/cave03.json";
	cave03a.LevelName = "levels/canyon/cave03.a.json";
	cave04 .LevelName = "levels/canyon/cave04.json";
	cave05 .LevelName = "levels/canyon/cave05.json";
	cave06 .LevelName = "levels/canyon/cave06.json";
	cave07 .LevelName = "levels/canyon/cave07.json";
	cave08 .LevelName = "levels/canyon/cave08.json";

	top01->LevelName = "levels/canyon/top01.json";
	top02 .LevelName = "levels/canyon/top02.json"; 
	top03 .LevelName = "levels/canyon/top03.json"; 
	top04 .LevelName = "levels/canyon/top04.json";
	top05 .LevelName = "levels/canyon/top05.json";
	top06 .LevelName = "levels/canyon/top06.json";
	top07 .LevelName = "levels/canyon/top07.json";
	top08 .LevelName = "levels/canyon/top08.json";

	top07 .AddConnection(top08);
	top06 .AddConnection(top07);
	top05 .AddConnection(top06);
	top04 .AddConnection(top05);
	top03 .AddConnection(top04);
	top02 .AddConnection(top03);
	top01->AddConnection(top02);

	cave07 .AddConnection(cave08);
	cave06 .AddConnection(cave07);
	cave05 .AddConnection(cave06);
	cave04 .AddConnection(cave05);
	cave03 .AddConnection(cave04);
	cave03 .AddConnection(cave03a);
	cave02 .AddConnection(cave03);
	cave01->AddConnection(cave02);

	canyon10 .AddConnection(canyon11);
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

	iw::JsonSerializer("C:/dev/IwEngine/Editor/assets/levels/forest.json").Write(*forest01);
	iw::JsonSerializer("C:/dev/IwEngine/Editor/assets/levels/canyon.json").Write(*canyon01);
	iw::JsonSerializer("C:/dev/IwEngine/Editor/assets/levels/cave.json").Write(*cave01);
	iw::JsonSerializer("C:/dev/IwEngine/Editor/assets/levels/top.json").Write(*top01);

	WorldLayout* forest = new WorldLayout(forest01);
	WorldLayout* canyon = new WorldLayout(canyon01);
	WorldLayout* cave   = new WorldLayout(cave01);
	WorldLayout* top    = new WorldLayout(top01);

	m_worlds.push_back(forest);

	m_worlds.push_back(canyon);
	m_worlds.push_back(cave);
	m_worlds.push_back(top);

	Bus->push<GotoLevelEvent>(startingLevel, 0);

	return 0;
}

bool LevelLayoutSystem::On(
	iw::ActionEvent& e)
{
	switch (e.Action) {
		case iw::val(Actions::RESET_LEVEL): {
			Bus->push<DeactivateLevelEvent>(CurrentLevelName());
			Bus->push<ActivateLevelEvent>  (CurrentLevelName());

			break;
		}
		case iw::val(Actions::GOTO_LEVEL): {
			GotoLevelEvent& event = e.as<GotoLevelEvent>();

			// Bus->push<DeactivateLevelEvent>(CurrentLevelName()); not sure if this is needed?

			FillWorlds(event.LevelName);

			Bus->push<UnloadLevelEvent>("All");

			Bus->push<LoadLevelEvent>(CurrentLevelName());
			Bus->push<LoadLevelEvent>(PreviousLevelName(), CurrentLevelName(), true);
			for (LevelLayout& connection : m_currentWorld->CurrentLevel->Connections) {
				Bus->push<LoadLevelEvent>(connection.LevelName, CurrentLevelName());
			}

			Bus->push<ActivateLevelEvent>(CurrentLevelName(), "", 101, event.InPosition);

			break;
		}
		case iw::val(Actions::GOTO_CONNECTED_LEVEL): {
			GotoConnectedLevelEvent& event = e.as<GotoConnectedLevelEvent>();

			Bus->push<DeactivateLevelEvent>(CurrentLevelName());

			// If we are moving to the next full level, unload previous level before moving
			if (event.Index == 1) {
				Bus->push<UnloadLevelEvent>(PreviousLevelName());

				// Unload all but main connections for current level before moving
				for (int i = 1; i < m_currentWorld->CurrentLevel->Connections.size(); i++) {
					Bus->push<UnloadLevelEvent>(m_currentWorld->CurrentLevel->Connections.at(i).LevelName);
				}
			}

			std::string from = CurrentLevelName();

			// If we are moving to previous level, load previous' previous level
			if (event.Index < 0) {
				if (m_currentWorld->CurrentLevel->Connections.size() > 0) {
					Bus->push<UnloadLevelEvent>(m_currentWorld->CurrentLevel->Connections.at(0).LevelName);
				}

				m_currentWorld->ToPreviousLevel(); // this is wrong somehow

				Bus->push<LoadLevelEvent>(PreviousLevelName(), CurrentLevelName(), true);
			}

			else {
				m_currentWorld->ToNextLevel(event.Index - 1);
			}

			// Load all connections for new level
			for (LevelLayout& connection : m_currentWorld->CurrentLevel->Connections) {
				Bus->push<LoadLevelEvent>(connection.LevelName, CurrentLevelName());
			}
			
			// Activate new level
			Bus->push<ActivateLevelEvent>(CurrentLevelName(), from, event.Index, 0); // get inposition from event

			break;
		}
	}

	if (   e.Action == iw::val(Actions::GOTO_LEVEL)
		|| e.Action == iw::val(Actions::GOTO_CONNECTED_LEVEL))
	{
		Bus->push<GameSave>();
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

std::string& LevelLayoutSystem::CurrentLevelName() {
	return m_currentWorld->CurrentLevel->LevelName;
}

std::string LevelLayoutSystem::PreviousLevelName() {
	if (m_currentWorld->PreviousLevels.size() == 0) {
		return "";
	}

	return m_currentWorld->PreviousLevels.top()->LevelName;
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
