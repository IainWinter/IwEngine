#pragma once

#include "iw/engine/Layer.h"
#include "iw/engine/Window.h"
#include "imgui/imgui.h"

namespace iw {
namespace Engine {
	class ImGuiLayer
		: public Layer
	{
	private:
		IWindow* m_window;
		void* m_context;

		ref<RenderTarget> viewportRT;

	public:
		IWENGINE_API ImGuiLayer(
			IWindow* window = nullptr);

		IWENGINE_API ~ImGuiLayer();

		IWENGINE_API int Initialize() override;

		IWENGINE_API void OnPush() override;
		IWENGINE_API void OnPop() override;

		IWENGINE_API void Destroy() override;
		IWENGINE_API void ImGui()   override;

		IWENGINE_API bool On(WindowResizedEvent& e) override;
		IWENGINE_API bool On(MouseMovedEvent&    e) override;
		IWENGINE_API bool On(MouseWheelEvent&    e) override;
		IWENGINE_API bool On(MouseButtonEvent&   e) override;

		IWENGINE_API bool On(KeyEvent& e) override;

		IWENGINE_API bool On(OsEvent& e) override;

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
