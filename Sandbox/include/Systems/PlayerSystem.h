#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"

class PlayerSystem
	: public IW::System<IW::Transform, Player>
{
public:
	PlayerSystem(
		IW::Space& space,
		IW::Graphics::Renderer& renderer);

	void Update(
		IW::EntityComponentArray& view) override;
};
