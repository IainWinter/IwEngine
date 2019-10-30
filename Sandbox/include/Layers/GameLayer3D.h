#pragma once

#include "iw/engine/Layer.h"

class GameLayer3D
	: public IwEngine::Layer
{
private:
	IW::Mesh* treeMeshs;
	iwu::ref<IW::Material>  treeMaterial;
	iwm::vector2 mouse;

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
