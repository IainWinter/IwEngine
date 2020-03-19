#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Dynamics/Rigidbody.h"
#include "Events/ActionEvents.h"
#include "iw/graphics/Model.h"

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
	iw::ref<iw::Model> m_playerModel;
	iw::vector3 movement;
	bool dash;
	bool enabled;

public:
	PlayerSystem();

	int Initialize() override;

	void Update(
		iw::EntityComponentArray& view) override;

	void FixedUpdate(
		iw::EntityComponentArray& view) override;

	bool On(iw::KeyEvent& event);
	bool On(iw::CollisionEvent& event);
	bool On(iw::ActionEvent& event);
};
