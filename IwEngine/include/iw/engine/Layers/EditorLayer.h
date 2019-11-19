#pragma once

#include "iw/engine/Layer.h"

namespace IW {
inline namespace Engine {
	class IWENGINE_API EditorLayer
		: public Layer
	{
	private:

	public:
		EditorLayer(
			IW::Space& space,
			IW::Renderer& renderer,
			IW::AssetManager& asset,
			iw::eventbus& bus);

		void PostUpdate() override;
		void ImGui() override;

		// Input events

		bool On(IW::MouseWheelEvent& event) override;
		bool On(IW::MouseMovedEvent& event) override;
		bool On(IW::MouseButtonEvent& event) override;
		bool On(IW::KeyEvent& event) override;
		bool On(IW::KeyTypedEvent& event) override;

		// Window events

		bool On(IW::WindowResizedEvent& event) override;
	};
}
}
