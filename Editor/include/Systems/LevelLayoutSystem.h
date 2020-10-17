#pragma once

#include "iw/engine/System.h"
#include <vector>
#include <stack>

#include "Events/ActionEvents.h"
#include "Components/LevelLayout.h"

class LevelLayoutSystem
	: public iw::SystemBase
{
private:
	struct WorldLayout {
		std::stack<LevelLayout*> PreviousLevels;
		LevelLayout* CurrentLevel;

		WorldLayout(
			LevelLayout* currentLevel)
			: CurrentLevel(currentLevel)
		{}

		void ToNextLevel(
			unsigned index);

		void ToPreviousLevel();

		void FillWorld(
			const std::string& untilLevel);

		void UnwindWorld();
	};

	std::vector<WorldLayout*> m_worlds;
	WorldLayout* m_currentWorld;

public:
	LevelLayoutSystem();

	int Initialize();

	bool On(iw::ActionEvent& e);
private:
	void FillWorlds(
		const std::string& untilLevel);
};
