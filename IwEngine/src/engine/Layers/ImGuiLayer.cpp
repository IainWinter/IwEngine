#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/log/logger.h"

#include "OpenGL/imgui_impl_opengl3.h"
#include "Windows/imgui_impl_win32.h"
#include "iw/engine/Time.h"

#include "../extern/imgui/src/imgui_demo.cpp"

#include "glm/gtx/compatibility.hpp"

//extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace iw {
namespace Engine {
	ImGuiLayer::ImGuiLayer(
		IWindow* window)
		: Layer("ImGui")
		, m_window(window)
		, m_context(nullptr)
	{
		target = REF<RenderTarget>();
		target->AddTexture(REF<Texture>(1280, 720));
		target->AddTexture(REF<Texture>(1280, 720, TEX_2D, DEPTH, FLOAT));

		aspect = (float)target->Height() / target->Width();
	}

	ImGuiLayer::~ImGuiLayer() {}

	int ImGuiLayer::Initialize() {
		IMGUI_CHECKVERSION();

		m_context = ImGui::CreateContext();
		//Bind();

		ImGui::StyleColorsDark();


		auto& io = ImGui::GetIO();
		io.ConfigFlags = ImGuiConfigFlags_DockingEnable;
		//if(m_window) io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImFont* pFont = io.Fonts->AddFontFromFileTTF("assets/fonts/ttf/verdana.ttf", 15);
		//ImGui::PushFont(pFont);

		if (m_window) ImGui_ImplWin32_Init(m_window->Handle(), m_window->Context());
		ImGui_ImplOpenGL3_Init("#version 450");

		//Renderer->SetDefaultTarget(target);

		return 0;
	}

	void ImGuiLayer::OnPush() {
		if(m_context)
			Renderer->SetDefaultTarget(target);
	}

	void ImGuiLayer::OnPop() {
		Renderer->SetDefaultTarget(nullptr);
	}

	void ImGuiLayer::Destroy() {
		ImGui_ImplOpenGL3_Shutdown();
		if (m_window) ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::ImGui() {
		ImGui::GetIO().DeltaTime = Time::DeltaTime();

		if (m_window) {
			glm::vec2 pos = Mouse::ScreenPos();

			ImGui::GetIO().MousePos.x = pos.x;
			ImGui::GetIO().MousePos.y = pos.y;
		}

		//ImGui::Begin("ImGui Layer");

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
		//	1000.0f / ImGui::GetIO().Framerate,
		//	ImGui::GetIO().Framerate);

		//ImGui::PlotLines("Fps", iw::Time::Times().first, iw::Time::Times().second);

		//ImGui::End();

	}
	
	std::unordered_map<std::string, float> m_smooth;
	float smoothing;

	void PrintTimes(
		iw::log_time& times)
	{
		m_smooth[times.name] = glm::lerp(m_smooth[times.name], times.time * 1000, glm::min(1 - smoothing + .01f, 1.f));
		float time = m_smooth[times.name];

		if (times.children.size() == 0) {
			ImGui::Text("%s - %4.4fms", times.name.c_str(), m_smooth[times.name]);
		}

		else {
			int id = (times.my_level << sizeof(int) / 2) | times.my_index;

			if (ImGui::TreeNode(&id, "%s - %4.4fms", times.name.c_str(), m_smooth[times.name])) {
				for (iw::log_time& child : times.children) {
					PrintTimes(child);
				}

				ImGui::TreePop();
			}
		}
	}

	void ImGuiLayer::Begin() {
		// Frame

		ImGui_ImplOpenGL3_NewFrame();
		if (m_window) ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		// temp Dockspace

		ImGuiWindowFlags window_flags =
			  ImGuiWindowFlags_MenuBar
			| ImGuiWindowFlags_NoDocking
			| ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoCollapse
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoMove
			| ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoNavFocus;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

		ImGui::Begin("Dockspace", nullptr, window_flags);

		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiID dockspace_id = ImGui::GetID("Dockspace");

		ImGui::DockSpace(dockspace_id);

		if (ImGui::BeginMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				ImGui::Separator();
				if (ImGui::MenuItem("Exit", NULL, false)) {
					Console->QueueCommand("quit");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMenuBar();
		}

		// temp times

		ImGui::Begin("Times");

		ImGui::SliderFloat("Smooth", &smoothing, 0, 1, "%1.2f", .5);
		PrintTimes(LOG_GET_TIMES());

		ImGui::End();

		// temp Viewspace

		ImGui::Begin("Viewspace");

		//target->Resize(size.x, size.y);

		//ImVec2 size;
		//size.x = target->Tex(0)->Width();
		//size.y = target->Tex(0)->Height();

		if (target->Tex(0)->Handle()) {
			ImVec2 size = ImGui::GetWindowSize();
			size.y = size.x * aspect;

			unsigned id = target->Tex(0)->Handle()->Id();
			ImGui::Image((void*)id, size, ImVec2(0, 1), ImVec2(1, 0));
		}

		ImGui::End();
	}

	void ImGuiLayer::End() {
		// Dockspace

		ImGui::End();

		// Frame

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Update and Render additional Platform Windows
		// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
		if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			//m_window->ReleaseOwnership();

			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			
			m_window->TakeOwnership();
		}
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
		if (m_window) return false;

		if (e.Device == DeviceType::MOUSE) {
			auto& io = ImGui::GetIO();

			glm::vec2 screenPos = glm::vec2(e.X, e.Y)/*iw::Mouse::ScreenPos()*/;

			io.MousePos.x = (float)screenPos.x;
			io.MousePos.y = (float)screenPos.y;
		}

		return ImGui::GetIO().WantCaptureMouse;
	}

	bool ImGuiLayer::On(
		MouseWheelEvent& e)
	{
		if (e.Device == DeviceType::MOUSE) {
			auto& io = ImGui::GetIO();
			if (io.WantCaptureMouse) {
				io.MouseWheel += e.Delta;
			}
		}

		return ImGui::GetIO().WantCaptureMouse;
	}

	bool ImGuiLayer::On(
		MouseButtonEvent& e)
	{
		if (e.Device == DeviceType::MOUSE) {
			auto& io = ImGui::GetIO();
			if (io.WantCaptureMouse) {
				io.MouseDown[e.Button - LMOUSE] = e.State;
			}
		}

		return ImGui::GetIO().WantCaptureMouse;
	}

	bool ImGuiLayer::On(
		KeyEvent& e)
	{
		if (e.Device == DeviceType::KEYBOARD) {
			auto& io = ImGui::GetIO();
			if (io.WantCaptureKeyboard) {
				io.KeysDown[e.Button] = e.State;
			}
		}

		return ImGui::GetIO().WantCaptureKeyboard;
	}
	
	bool ImGuiLayer::On(
		OsEvent& e)
	{
		return false;// ImGui_ImplWin32_WndProcHandler(e.Handle, e.Message, e.WParam, e.LParam);
	}
}
}
