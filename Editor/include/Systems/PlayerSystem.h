#pragma once

#include "Events/ActionEvents.h"
#include "iw/engine/System.h"
#include "iw/graphics/Model.h"
#include "iw/reflect/Components/Player.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Camera.h"

class PlayerSystem
	: public iw::System<iw::Transform, iw::Rigidbody, Player>
{
public:
	struct Components {
		iw::Transform* Transform;
		iw::Rigidbody* Rigidbody;
		Player*    Player;
	};
private:
	// prefab info
	Player playerPrefab;
	iw::ref<iw::Model> m_playerModel;

	iw::Entity player;

	iw::vector3 movement;
	bool dash;

public:
	PlayerSystem();

	iw::Entity& GetPlayer() {
		return player;
	}

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	void FixedUpdate(
		iw::EntityComponentArray& view) override;

	bool On(iw::KeyEvent& event);
	bool On(iw::CollisionEvent& event);
	bool On(iw::ActionEvent& event);
};
