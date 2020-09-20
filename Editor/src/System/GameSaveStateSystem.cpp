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

GameSaveState* GameSaveStateSystem::GetState() {
	return m_state;
}
