#pragma once 

#include "iw/engine/Layer.h"
#include "iw/graphics/Shader.h"
#include "iw/graphics/Camera.h"

class RecingLayer
	: public iw::Layer
{
private:
	iw::ref<iw::Shader> shader;
	iw::Scene* scene;

public:
	RecingLayer();

	int Initialize() override;

	void PostUpdate() override;
	void FixedUpdate() override;

	void ImGui() override;
};
