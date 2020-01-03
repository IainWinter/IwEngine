#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/physics/Dynamics/Rigidbody.h"

class PlayerSystem
	: public IW::System<IW::Transform, IW::Rigidbody, Player>
{
public:
	struct Components {
		IW::Transform* Transform;
		IW::Rigidbody* Rigidbody;
		Player*    Player;
	};
private:
	iw::vector3 movement;
	bool dash;

public:
	PlayerSystem();

	void Update(
		IW::EntityComponentArray& view) override;

	void FixedUpdate(
		IW::EntityComponentArray& view) override;

	bool On(
		IW::KeyEvent& event);
};
