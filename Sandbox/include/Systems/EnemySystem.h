#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/common/Components/Transform.h"
#include "iw/graphics/Mesh.h"

class EnemySystem
	: public IW::System<IW::Transform, Enemy>
{
public:
	struct Components {
		IW::Transform* Transform;
		Enemy* Enemy;
	};
private:
	IW::Mesh* CircleMesh;

public:
	EnemySystem(
		IW::Mesh* circle);

	void Update(
		IW::EntityComponentArray& view) override;

	void Destroy() override;
};
