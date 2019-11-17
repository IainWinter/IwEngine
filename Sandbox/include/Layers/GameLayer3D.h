#pragma once

#include "iw/engine/Layer.h"

class GameLayer3D
	: public IW::Layer
{
private:
	iw::vector3 lightPositions[4];
	iw::vector3 lightColors[4];

	iw::ref<IW::Material> material;

public:
	GameLayer3D(
		IW::Space& space,
		IW::Renderer& renderer,
		IW::AssetManager& asset);

	int Initialize(
		IW::InitOptions& options) override;

	void PostUpdate() override;
	void ImGui() override;

	bool On(IW::MouseMovedEvent& event) override;
	bool On(IW::KeyEvent& event) override;
};
