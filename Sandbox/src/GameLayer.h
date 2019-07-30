#pragma once

#include "iw/engine/Layer.h"
#include "iw/entity/Space.h"
#include "iw/engine/Loaders/ModelLoader.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"

class GameLayer
	: public IwEngine::Layer
{
private:
	IwEntity::Space        space;
	IwEngine::ModelLoader  loader;
	IwRenderer::IDevice*   device;
	IwRenderer::IPipeline* pipeline;

public:
	GameLayer();
	~GameLayer();

	int  Initialize()  override;
	void Update()      override;
	void FixedUpdate() override;
};
