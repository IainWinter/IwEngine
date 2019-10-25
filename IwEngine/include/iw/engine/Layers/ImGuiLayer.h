#pragma once

#include "iw/engine/Layer.h"

namespace IwEngine {
	class IWENGINE_API ImGuiLayer
		: public Layer
	{
	private:
		void* m_context;
		//test
		int   counter = 0;

	public:
		ImGuiLayer(
			IwEntity::Space& space,
			IW::Graphics::Renderer& renderer);

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
			WindowResizedEvent& event) override;

		bool On(
			MouseMovedEvent& event) override;

		bool On(
			MouseButtonEvent& event) override;
	};
}
