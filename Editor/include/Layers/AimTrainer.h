#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"

class AimTrainerLayer
	: public iw::Layer
{
private:
	iw::ref<iw::Shader> shader;

public:
	AimTrainerLayer();

	int Initialize() override;

	void PostUpdate() override;

	void ImGui() override;
};
