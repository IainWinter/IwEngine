#pragma once

#include "iw/engine/Layer.h"
#include "imgui/imgui.h"

namespace IW {
inline namespace Engine {
	class ImGuiLayer
		: public Layer
	{
	private:
		void* m_context;

	public:
		IWENGINE_API ImGuiLayer();

		IWENGINE_API ~ImGuiLayer();

		IWENGINE_API int Initialize() override;

		IWENGINE_API void Update()  override;
		IWENGINE_API void Destroy() override;
		IWENGINE_API void ImGui()   override;

		IWENGINE_API bool On(
			IW::WindowResizedEvent& event) override;

		IWENGINE_API bool On(
			IW::MouseMovedEvent& event) override;

		IWENGINE_API bool On(
			IW::MouseButtonEvent& event) override;

		IWENGINE_API void Begin();
		IWENGINE_API void End();

		inline void BindContext() {
			ImGui::SetCurrentContext((ImGuiContext*)m_context);
		}

		inline void* GetContext() {
			return m_context;
		}
	};
}
}
