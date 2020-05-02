#pragma once

#include "Components/Consumable.h"

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Font.h"

#include <vector>

class ConsumableSystem
	: public iw::System<iw::Transform, Consumable>
{
public:
	struct Components {
		iw::Transform* Transform;
		Consumable* Item;
	};

public:
	ConsumableSystem();

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnConsumable(
		Consumable prefab);
};
