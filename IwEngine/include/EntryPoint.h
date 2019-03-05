#pragma once

#include "Application.h"
#include "logger.h"

extern IwEngine::Application* CreateApplication();

#ifdef IW_PLATFORM_WINDOWS

int CALLBACK WinMain(
	HINSTANCE handle,
	HINSTANCE prevhandle,
	LPSTR cmdline,
	int cmdshow)
{
	bool console = true;
	if (console) {
		AllocConsole();
		FILE *fo, *fe;
		freopen_s(&fo, "CONOUT$", "w", stdout);
		freopen_s(&fe, "CONERR$", "w", stderr);
	}

	IwEngine::Application* app = CreateApplication();
	app->Start();

	delete app;

	LOG_FLUSH();

	if (console) {
		FreeConsole();
	}
}

#endif

// Application Life Cycle
//
//  Constructor
//       |
//   Initilize
//       |
//     Start
//       |   
//      Run<---
//       |--->^
//       |
//     Stop
//       |
//    Destroy
//       |
//   Destructor
//
