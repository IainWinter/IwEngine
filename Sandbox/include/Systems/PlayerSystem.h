#pragma once

#include "Components/Player.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"

class PlayerSystem
	: public IwEngine::System<IwEngine::Transform, Player>
{
public:
	PlayerSystem(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue);

	~PlayerSystem();

	void Update(
		IwEntity::EntityComponentArray& view) override;
};
