#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"

class PlayerSystem
	: public IW::System<IW::Transform, Player>
{
private:
	iw::vector3 movement;
	bool dash;

public:
	PlayerSystem();

	void Update(
		IW::EntityComponentArray& view) override;

	bool On(
		IW::KeyEvent& event);
};
