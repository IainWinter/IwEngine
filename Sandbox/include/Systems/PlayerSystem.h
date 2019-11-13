#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"

class PlayerSystem
	: public IwEngine::System<IW::Transform, Player>
{
public:
	PlayerSystem(
		IwEntity::Space& space,
		IW::Graphics::Renderer& renderer);

	void Update(
		IwEntity::EntityComponentArray& view) override;
};
