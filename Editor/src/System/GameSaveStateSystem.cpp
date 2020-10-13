#include "Systems/GameSaveStateSystem.h"
#include "Events/ActionEvents.h"
#include "iw/reflection/serialization/JsonSerializer.h"
#include "iw/util/io/File.h"

#include "iw/reflect/Components/GameSaveState.h"

GameSaveStateSystem::GameSaveStateSystem()
	: iw::SystemBase("Game Save State")
	, m_state(new GameSaveState())
	, m_file("C:/dev/IwEngine/Editor/assets/game/save.json")
{}

GameSaveStateSystem::~GameSaveStateSystem() {
	delete m_state;
}

int GameSaveStateSystem::Initialize() {
	if (iw::FileExists(m_file)) {
		iw::JsonSerializer(m_file).Read(*m_state);
	}

	else {
		iw::JsonSerializer(m_file).Write(*m_state);
	}

	return 0;
}

bool GameSaveStateSystem::On(
	iw::ActionEvent& e)
{
	if (e.Action == iw::val(Actions::GAME_SAVE)) {
		iw::JsonSerializer(m_file, true).Write(*m_state);
	}

	return false;
}

GameSaveState* GameSaveStateSystem::GetState() {
	return m_state;
}
