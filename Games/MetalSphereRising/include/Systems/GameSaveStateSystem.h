#pragma once

#include "iw/engine/System.h"
#include "Components/GameSaveState.h"

class GameSaveStateSystem
	: public iw::SystemBase
{
private:
	GameSaveState* m_state;
	std::string m_file;

public:
	GameSaveStateSystem();
	~GameSaveStateSystem();

	int Initialize() override;

	bool On(iw::ActionEvent& e) override;

	GameSaveState* GetState();
};
