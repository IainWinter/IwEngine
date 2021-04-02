#pragma once

#include "Components/Item.h"
#include "Components/GameSaveState.h"

#include "iw/common/Components/Transform.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"

class ItemSystem
	: public iw::SystemBase
{
private:
	iw::Model m_noteModel;
	GameSaveState* m_saveState;

public:
	ItemSystem(
		GameSaveState* saveState);

	int Initialize() override;

	bool On(iw::ActionEvent& e)    override;
	bool On(iw::CollisionEvent& e) override;
private:
	iw::Transform* SpawnItem(
		Item prefab,
		iw::vector3 position,
		std::string saveState);
};
