#pragma once

#include "iw/engine/Layer.h"

class GameLayer
	: public IwEngine::Layer
{
private:
public:
	GameLayer();
	~GameLayer();

	int  Initialize() override;
	void Update()     override;
};
