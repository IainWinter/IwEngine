#pragma once

#include "iw/engine/Layer.h"
#include "iw/entity/Space.h"

class GameLayer
	: public IwEngine::Layer
{
private:
	IwEntity::Space space;

public:
	GameLayer();
	~GameLayer();

	int  Initialize() override;
	void Update()     override;
};
