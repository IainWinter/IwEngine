#pragma once

#include "iw/engine/Layer.h"
#include "imgui/imgui.h"

namespace IW {
namespace Engine {
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

		IWENGINE_API bool On(WindowResizedEvent& e) override;
		IWENGINE_API bool On(MouseMovedEvent&    e) override;
		IWENGINE_API bool On(MouseButtonEvent&   e) override;

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

	using namespace Engine;
}
