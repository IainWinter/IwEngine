#include "iw/engine/EntryPoint.h"
#include <string>

#include "iw/engine/Events/Events.h"
#include "iw/engine/Layers/ImGuiLayer.h"
#include "iw/engine/Layers/ToolLayer.h"

//#include "iw/math/matrix.h"

using GETAPP_FUNC = iw::Application*(*)();

void Reload(std::wstring dll, HINSTANCE& hInst) {
	if (hInst) {
		FreeLibrary(hInst);
	}

	hInst = LoadLibrary(dll.c_str());

	if (!hInst) {
		LOG_ERROR << "Failed to load library " << dll.c_str();
	}
}

template<typename _t>
_t LoadFunction(HINSTANCE hInst, std::string function) {
	_t funcPtr = (_t)GetProcAddress(hInst, function.c_str());

	if (!funcPtr) {
		LOG_ERROR << "Failed to load function " << function;
	}

	return funcPtr;
}

class EditorApp : public iw::Application {
private:
	iw::InitOptions& options;

	iw::ImGuiLayer* imgui = nullptr;
	iw::Layer* toolbox = nullptr;
	iw::Layer* sandbox = nullptr;

	std::wstring projectName          = L"PixelArt";
	std:: string projectMainLayerName =  "Pixelation";

public:
	iw::Application* m_gameNew = nullptr;
	iw::Application* m_game    = nullptr;

	EditorApp(iw::InitOptions& options): options(options) {}

	int Initialize(iw::InitOptions& options) {
		return InitGame();
	}

	void Run() {
		while (m_game) {
			if (!m_game->Window()->Handle()) {
				InitGame();
			}

			m_game->Run();
			
			delete m_game;
			m_game = m_gameNew; m_gameNew = nullptr;
		}
	}

	void ReloadGame() {


		
		//iw::mat<2, 1> dasd;


		//iw::matrix<2, 2, float> mat = { 1, 2, 3, 4 };
		//iw::matrix<1, 2, float> vec{ 1, 2 };

		//iw::det(mat);

		//iw::vector<2> row = mat.row(0);



		//iw::matrix<1, 2> m = mat * vec;

		//LOG_INFO << m;

		std::wstringstream buf;

#ifdef IW_DEBUG
		buf << L"C:/dev/IwEngine/Games/";
		buf << projectName;
		buf << "/bin/Debug.windows.x86_64/";
		buf << projectName;
		buf << ".dll";
#else
		buf << L"C:/dev/IwEngine/Games/";
		buf << projectName;
		buf << "/bin/Release.windows.x86_64/";
		buf << projectName;
		buf << ".dll";
#endif

		std::wstring path = buf.str();

		HINSTANCE gameInst;
		Reload(path.c_str(), gameInst);
		m_gameNew = LoadFunction<GETAPP_FUNC>(gameInst, "GetApplicationForEditor")();
	}

	int InitGame() {



		//iw::matrix<2, 2> mat {
		//	1, 2, 
		//	3, 4
		//};

		//iw::invert(mat);

		sandbox = m_game->GetLayer(projectMainLayerName.c_str());
		
		if (imgui)         m_game->PushLayer(imgui);
		else       imgui = m_game->PushLayer<iw::ImGuiLayer>();

		int err = m_game->Initialize(options);

		if (toolbox)           m_game->PushLayer(toolbox);
		else         toolbox = m_game->PushLayer<iw::ToolLayer>(sandbox->GetMainScene());

		err = toolbox->Initialize();
		m_game->PopLayer(toolbox);

		if (imgui) {
			imgui->BindContext();
		}

		err = imgui->Initialize();
		m_game->PopLayer(imgui);

		m_game->Console->AddHandler([&](const iw::Command& command) {
			if (command.Verb == "reload") {
				m_game->PopLayer(imgui);
				m_game->PopLayer(toolbox);
				m_game->Bus->push<iw::InputCommandEvent>("quit");
				ReloadGame();
			}

			else if (command.Verb == "toolbox") { // in editor
				bool dev = m_game->GetLayer("Toolbox") != nullptr;
				if (dev) {
					m_game->PopLayer(toolbox);
					//m_game->PushLayer(sandbox);
				}

				else {
					m_game->PushLayer(toolbox);
					//m_game->PopLayer(sandbox);
				}
			}

			else if (command.Verb == "imgui") {
				if (m_game->GetLayer("ImGui") == nullptr) {
					m_game->PushLayer(imgui);
					m_game->Bus->push<iw::WindowResizedEvent>(m_game->Window()->Id(), m_game->Renderer->Width(), m_game->Renderer->Height());
				}

				else {
					m_game->PopLayer(imgui);
				}
			}

			return false;
		});

		return err;
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	EditorApp* app = new EditorApp(options);
	app->ReloadGame();

	app->m_game = app->m_gameNew;

	return app;
}
