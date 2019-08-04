#pragma once

#include "Components/Enemy.h"
#include "iw/engine/System.h"
#include "iw/engine/Components/Transform.h"
#include "iw/graphics/Asset/ModelData.h"

class EnemySystem
	: public IwEngine::System<IwEngine::Transform, Enemy>
{
private:
	IwGraphics::ModelData* Circle;

public:
	EnemySystem(
		IwEntity::Space& space,
		IwGraphics::RenderQueue& renderQueue,
		IwGraphics::ModelData* circle);

	~EnemySystem();

	void Update(
		View& view) override;
};
