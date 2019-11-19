#include "iw/engine/Layers/EditorLayer.h"
#include "iw/engine/Components/Model.h"
#include "imgui/imgui.h"

namespace IW {
	EditorLayer::EditorLayer()
		: Layer("Editor")
	{}

	void EditorLayer::PostUpdate() {

		Renderer->BeginScene();

		for (auto entity : Space->Query<IW::Transform, IW::ModelComponent>()) {
			auto [t, m] = entity.Components.TieTo<IW::Transform, IW::ModelComponent>();

			for (size_t i = 0; i < m->MeshCount; i++) {
				Renderer.DrawMesh(t, &m->Meshes[i]);
			}
		}

		Renderer.End();
	}

	void EditorLayer::ImGui() {
		ImGui::Begin("Editor");



		ImGui::End();
	}

	bool EditorLayer::On(IW::MouseWheelEvent& event)
	{
		return true;
	}

	bool EditorLayer::On(IW::MouseMovedEvent& event)
	{
		return true;
	}

	bool EditorLayer::On(IW::MouseButtonEvent& event)
	{
		return true;
	}

	bool EditorLayer::On(IW::KeyEvent& event)
	{
		return true;
	}

	bool EditorLayer::On(IW::KeyTypedEvent& event)
	{
		return true;
	}

	bool EditorLayer::On(IW::WindowResizedEvent& event)
	{
		return true;
	}
}
