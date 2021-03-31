#include "iw/engine/EntryPoint.h"
#include <string>

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
			m_game = m_gameNew;
			m_gameNew = nullptr;
		}
	}

	void ReloadGame() {
		HINSTANCE gameInst;
		Reload(L"a_wEditor.dll", gameInst);
		m_gameNew = LoadFunction<GETAPP_FUNC>(gameInst, "GetApplicationForEditor")();
	}

	int InitGame() {
		int err = m_game->Initialize(options);

		m_game->Console->AddHandler([&](const iw::Command& e) {
			if (e.Verb == "quit") {
				ReloadGame();
			}

			else if (e.Verb == "toolbox") {
				m_game->Bus->push<iw::InputCommandEvent>("quit");
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

