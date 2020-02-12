#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Dynamics/Rigidbody.h"

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
	iw::vector3 movement;
	bool dash;

public:
	PlayerSystem();

	void Update(
		iw::EntityComponentArray& view) override;

	void FixedUpdate(
		iw::EntityComponentArray& view) override;

	bool On(iw::KeyEvent& event);
	bool On(iw::CollisionEvent& event);
};
