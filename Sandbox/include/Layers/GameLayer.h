#pragma once

#include "iw/engine/Layer.h"
#include "iw/entity/Space.h"
#include "iw/graphics/RenderQueue.h"
#include "iw/graphics/Loaders/ModelLoader.h"
#include "iw/renderer/Device.h"
#include "iw/renderer/Pipeline.h"
#include "iw/graphics/Material.h"

class GameLayer
	: public IwEngine::Layer
{
private: // tmp
	IW::Mesh* line;

public:
	GameLayer(
		IwEntity::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset);

	int Initialize(
		IwEngine::InitOptions& options) override;

	void PostUpdate()  override;
	void FixedUpdate() override;
	void ImGui()       override;
};
