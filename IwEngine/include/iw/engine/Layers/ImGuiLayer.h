#pragma once

#include "iw/engine/Layer.h"

namespace IW {
inline namespace Engine {
	class IWENGINE_API ImGuiLayer
		: public Layer
	{
	private:
		void* m_context;
		//test
		int   counter = 0;

	public:
		ImGuiLayer(
			IW::Space& space,
			IW::Renderer& renderer,
			IW::AssetManager& asset,
			iw::eventbus& bus);

		~ImGuiLayer();

		int Initialize(
			InitOptions& options) override;

		void Update()     override;
		void Destroy()    override;
		void ImGui()      override;

		void Begin();
		void End();

		inline void* GetContext() {
			return m_context;
		}

		bool On(
			IW::WindowResizedEvent& event) override;

		bool On(
			IW::MouseMovedEvent& event) override;

		bool On(
			IW::MouseButtonEvent& event) override;
	};
}
}
