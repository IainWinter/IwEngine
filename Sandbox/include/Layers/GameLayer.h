#pragma once

#include "iw/engine/Layer.h"
#include "iw/entity/Space.h"
#include "iw/graphics/RenderQueue.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"

class GameLayer
	: public IwEngine::Layer
{
private:
	IwGraphics::ModelLoader loader;
	IwRenderer::IDevice*    device;
	IwRenderer::IPipeline*  pipeline;
	IwGraphics::RenderQueue RenderQueue;

public:
	GameLayer(
		IwEntity::Space& space);

	~GameLayer();

	int Initialize(
		IwEngine::InitOptions& options)  override;

	void Update()      override;
	void FixedUpdate() override;
	void ImGui()       override;
};
