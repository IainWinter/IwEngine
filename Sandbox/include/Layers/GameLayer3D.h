#pragma once

#include "iw/engine/Layer.h"

class GameLayer3D
	: public IwEngine::Layer
{
private:
	iwm::vector3 lightPositions[4];
	iwm::vector3 lightColors[4];

	iwu::ref<IW::Material> material;

public:
	GameLayer3D(
		IwEntity::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset);

	int Initialize(
		IwEngine::InitOptions& options) override;

	void PostUpdate() override;
	void ImGui() override;

	bool On(
		IwEngine::MouseMovedEvent& event) override;
};
