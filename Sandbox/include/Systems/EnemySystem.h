#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/data/Components/Transform.h"
#include "iw/graphics/Asset/ModelData.h"

class EnemySystem
	: public IwEngine::System<IwEngine::Transform, Enemy>
{
private:
	IwGraphics::ModelData* CircleData;
	IwGraphics::Mesh*      CircleMesh;

public:
	EnemySystem(
		IwEntity::Space& space,
		IwGraphics::Renderer& renderer,
		IwGraphics::ModelData* circle);

	~EnemySystem();

	int Initialize() override;

	void Update(
		IwEntity::EntityComponentArray& view) override;

	void Destroy() override;
};
