#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"
#include "iw/graphics/Mesh.h"

class EnemySystem
	: public IW::System<IW::Transform, Enemy>
{
private:
	IW::Mesh* CircleMesh;

public:
	EnemySystem(
		IW::Space& space,
		IW::Graphics::Renderer& renderer,
		IW::Mesh* circle);

	void Update(
		IW::EntityComponentArray& view) override;

	void Destroy() override;
};
