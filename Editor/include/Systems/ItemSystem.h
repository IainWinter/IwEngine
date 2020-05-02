#pragma once

#include "Components/Item.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Mesh.h"

class ItemSystem
	: public iw::SystemBase
{
private:
	iw::Mesh m_noteMesh;
	Item m_prefab;

public:
	ItemSystem();

	int Initialize() override;

	bool On(iw::ActionEvent& e)    override;
	bool On(iw::CollisionEvent& e) override;
private:
	iw::Transform* SpawnItem(
		Item prefab,
		iw::vector3 position);
};
