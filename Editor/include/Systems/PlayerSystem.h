#pragma once

#include "Events/ActionEvents.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"
#include "iw/reflect/Components/Player.h"
#include "iw/physics/Collision/CollisionObject.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Camera.h"

class PlayerSystem
	: public iw::System<iw::Transform, iw::Rigidbody, Player>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Rigidbody* Object;
		Player* Player;
	};
private:
	// prefab info
	Player playerPrefab;
	iw::ref<iw::Model> m_playerModel;

	iw::Entity m_player;
	iw::Transform*& m_worldTransform;

	unsigned m_activeConsumable;

public:
	PlayerSystem(
		iw::Transform*& worldTransform);

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	void OnPush() override;

	bool On(iw::KeyEvent& e);
	bool On(iw::ActionEvent& eent);

	iw::Entity& GetPlayer() {
		return m_player;
	}
};
