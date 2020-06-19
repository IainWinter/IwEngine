#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/log/logger.h"

#include "OpenGL/imgui_impl_opengl3.h"
#include "iw/engine/Time.h"

#include "..\extern\imgui\src\imgui_demo.cpp"

namespace iw {
namespace Engine {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui")
		, m_context(nullptr)
	{}

	ImGuiLayer::~ImGuiLayer() {}

	int ImGuiLayer::Initialize() {
		IMGUI_CHECKVERSION();

		m_context = ImGui::CreateContext();
		//Bind();

		ImGui::StyleColorsDark();
		ImGui_ImplOpenGL3_Init("#version 410");

		auto& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

		return 0;
	}

	void ImGuiLayer::Destroy() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Update() {
		ImGui::GetIO().DeltaTime = Time::DeltaTime();
	}

	void ImGuiLayer::ImGui() {
		// test

		static bool dockspace_open = true;
		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruDockspace)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockspace_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", NULL, false)) {
					Console->QueueCommand("quit");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		ImGui::Begin("ImGui Layer");

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
			1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate);

		ImGui::PlotLines("Fps", iw::Time::Times().first, iw::Time::Times().second);

		ImGui::End();

		ImGui::End();
	}

	void ImGuiLayer::Begin() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() {
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool ImGuiLayer::On(
		WindowResizedEvent& e)
	{
		auto& io = ImGui::GetIO();
		io.DisplaySize.x = (float)e.Width;
		io.DisplaySize.y = (float)e.Height;

		return false;
	}

	bool ImGuiLayer::On(
		MouseMovedEvent& e)
	{
		if (e.Device == DeviceType::MOUSE) {
			auto& io = ImGui::GetIO();
			io.MousePos.x = (float)e.X;
			io.MousePos.y = (float)e.Y;

			return io.WantCaptureMouse;
		}

		if (e.Device == DeviceType::RAW_MOUSE) {
			return ImGui::GetIO().WantCaptureMouse;
		}

		return false;
	}

	bool ImGuiLayer::On(
		MouseButtonEvent& e)
	{
		if (e.Device == DeviceType::MOUSE) {
			auto& io = ImGui::GetIO();
			if (io.WantCaptureMouse) {
				io.MouseDown[e.Button - LMOUSE] = e.State;
				return true;
			}

			return io.WantCaptureMouse;
		}

		if (e.Device == DeviceType::RAW_MOUSE) {
			return ImGui::GetIO().WantCaptureMouse;
		}

		return false;
	}
}
}
