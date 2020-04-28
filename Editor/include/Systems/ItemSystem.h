#pragma once

#include "Components/Item.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Mesh.h"

class ItemSystem
	: public iw::System<iw::Transform, iw::CollisionObject, Item>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::CollisionObject* Object;
		Item* Item;
	};
private:
	iw::Mesh m_noteMesh;
	Item m_prefab;

public:
	ItemSystem();

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	bool On(iw::ActionEvent& e)    override;
	bool On(iw::CollisionEvent& e) override;
private:
	iw::Transform* SpawnItem(
		Item prefab,
		iw::vector3 position);
};
