#pragma once

#include "iw/engine/System.h"
#include "Events/ActionEvents.h"
#include "Components/LevelLayout.h"
#include <stack>

class LevelLayoutSystem
	: public iw::SystemBase
{
private:
	struct WorldLayout {
		std::stack<LevelLayout*> PreviousLevels;
		LevelLayout* CurrentLevel;

		void ToNextLevel(
			unsigned index);

		void ToPreviousLevel();

		void FillWorld(
			LevelLayout* layout,
			const std::string& untilLevel);
	};

	std::vector<WorldLayout*> m_worlds;
	WorldLayout* m_currentWorld;

public:
	LevelLayoutSystem();

	int Initialize() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:

};
