#pragma once

#include "iw/engine/System.h"
#include "Events/ActionEvents.h"
#include "Components/LevelLayout.h"
#include <vector>
#include <stack>

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
	};

	std::vector<WorldLayout*> m_worlds;
	WorldLayout* m_currentWorld;

public:
	LevelLayoutSystem();

	int Initialize() override;
	void Update() override;

	bool On(iw::ActionEvent& e) override;
private:
	void FillWorlds(
		const std::string& untilLevel);
};
