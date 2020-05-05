#pragma once

#include "Components/Consumable.h"

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Font.h"

#include <vector>

class ConsumableSystem
	: public iw::System<iw::Transform, Consumable>
{
private:
	int m_activeConsumable;
	bool m_used;
	bool m_usingItem;

	std::vector<Consumable> m_prefabs;
	iw::Mesh m_slowmo;

	iw::Entity& m_target;

public:
	struct Components {
		iw::Transform* Transform;
		Consumable* Consumable;
	};

public:
	ConsumableSystem(
		iw::Entity& target);

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::ActionEvent& e) override;
private:
	iw::Transform* SpawnConsumable(
		Consumable prefab);
};
