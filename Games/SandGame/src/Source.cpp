#include "iw/engine/EntryPoint.h"
#include <string>

using GETAPP_FUNC = iw::Application*(*)();

void Reload(std::wstring dll, HINSTANCE& hInst) {
	if (hInst) {
		FreeLibrary(hInst);
	}

	hInst = LoadLibrary(dll.c_str());

	if (!hInst) {
		//LOG_ERROR << "Failed to load library " << dll;
		// set hinst to set value? or just null
	}
}

template<typename _t>
_t LoadFunction(HINSTANCE hInst, std::string function) {
	_t funcPtr = (_t)GetProcAddress(hInst, function.c_str());

	if (!funcPtr) {
		//LOG_ERROR << "Failed to load function " << function;
	}

	return funcPtr;
}

class EditorApp : public iw::Application {
private:
	iw::Application* m_game = nullptr;

public:
	int Initialize(iw::InitOptions& options) {
		int err = m_game->Initialize(options);
		if (err) return err;
		
		//m_game->PushLayer<TooLayer>();

		m_game->Console->AddHandler([&](const iw::Command& e) {

			ReloadGame();

			return false;
		});

		return 0;
	}

	void Run() {
		m_game->Run();
	}

	void ReloadGame() {
		if (m_game) {
			m_game->Console->QueueCommand("quit");
		}

		HINSTANCE gameInst;
		Reload(L"a_wEditor.dll", gameInst);
		m_game = LoadFunction<GETAPP_FUNC>(gameInst, "GetApplicationForEditor")();
	}
};

iw::Application* CreateApplication(
	iw::InitOptions& options)
{
	EditorApp* app = new EditorApp();
	app->ReloadGame();

	return app;
}

