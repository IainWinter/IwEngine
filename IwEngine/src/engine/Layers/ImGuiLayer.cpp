#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/log/logger.h"
#include "imgui/imgui.h"
#include "OpenGL/imgui_impl_opengl3.h"
#include "iw/engine/Time.h"

namespace IW {
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui")
	{}

	ImGuiLayer::~ImGuiLayer() {}

	int ImGuiLayer::Initialize(
		InitOptions& options)
	{
		IMGUI_CHECKVERSION();

		options.ImGuiContext = ImGui::CreateContext();
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
		ImGui::Begin("ImGui Layer");

		if (ImGui::Button("Button"))
			counter++;

		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f
			/ ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

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
		IW::WindowResizedEvent& e)
	{
		auto& io = ImGui::GetIO();
		io.DisplaySize.x = (float)e.Width;
		io.DisplaySize.y = (float)e.Height;

		return false;
	}

	bool ImGuiLayer::On(
		IW::MouseMovedEvent& event)
	{
		auto& io = ImGui::GetIO();
		io.MousePos.x = (float)event.X;
		io.MousePos.y = (float)event.Y;

		return io.WantCaptureMouse;
	}

	bool ImGuiLayer::On(
		IW::MouseButtonEvent& event)
	{
		auto& io = ImGui::GetIO();

		if (io.WantCaptureMouse) {
			io.MouseDown[event.Button - IW::LMOUSE] = event.State;
			return true;
		}

		return io.WantCaptureMouse;
	}
}
