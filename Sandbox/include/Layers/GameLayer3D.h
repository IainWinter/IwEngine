#pragma once

#include "iw/engine/Layer.h"

class GameLayer3D
	: public IwEngine::Layer
{
private:
	IW::Mesh* treeMeshs;
	iwm::vector2 mouse;

public:
	GameLayer3D(
		IwEntity::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset);

	int Initialize(
		IwEngine::InitOptions& options) override;

	void PostUpdate() override;

	bool On(
		IwEngine::MouseMovedEvent& event) override;
};
