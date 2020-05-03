#pragma once

#include "Events/ActionEvents.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"
#include "iw/reflect/Components/Player.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Camera.h"

class PlayerSystem
	: public iw::System<iw::Transform, iw::CollisionObject, Player>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::CollisionObject* Object;
		Player* Player;
	};
private:
	// prefab info
	Player playerPrefab;
	iw::ref<iw::Model> m_playerModel;

	iw::Entity player;

	bool left, right, up, down, dash, sprint, transition;
	iw::vector3 transitionStartPosition;
	iw::vector3 transitionTargetPosition;
	float begin;

	unsigned m_activeConsumable;

public:
	PlayerSystem();

	iw::Entity& GetPlayer() {
		return player;
	}

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	void OnPush() override;

	bool On(iw::KeyEvent& event);
	bool On(iw::CollisionEvent& event);
	bool On(iw::ActionEvent& event);
};
