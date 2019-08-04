#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/engine/Components/Transform.h"

class PlayerSystem
	: public IwEngine::System<IwEngine::Transform, Player>
{
public:
	PlayerSystem(
		IwEntity::Space& space);

	~PlayerSystem();

	void Update(
		View& view) override;
};
