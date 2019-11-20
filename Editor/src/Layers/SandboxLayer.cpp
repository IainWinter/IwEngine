#include "Layers/SandboxLayer.h"

namespace IW {
	SandboxLayer::SandboxLayer()
		: Layer("Sandbox")
	{}

	void SandboxLayer::PostUpdate() {
		Renderer->BeginScene();

		Renderer->EndScene();
	}
}

