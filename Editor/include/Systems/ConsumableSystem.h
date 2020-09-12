#pragma once

#include "Components/Consumable.h"

#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Font.h"
#include "iw/graphics/Model.h"

#include <vector>

class ConsumableSystem
	: public iw::SystemBase
{
private:
	std::vector<Consumable> m_prefabs;
	iw::Mesh m_mesh;

	std::vector<iw::EntityHandle> m_used;
	std::vector<iw::EntityHandle> m_justGot;

	iw::EntityHandle m_active;
	bool m_isActive;

	iw::Entity& m_target;

public:
	ConsumableSystem(
		iw::Entity& target);

	int Initialize() override;
	void Update() override;
	
	bool On(iw::ActionEvent& e) override;
private:
	iw::EntityHandle SpawnConsumable(
		Consumable prefab);
};
