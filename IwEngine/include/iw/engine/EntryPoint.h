#pragma once

#include "Application.h"
#include "iw/log/logger.h"

extern IwEngine::Application* CreateApplication();

#ifdef IW_PLATFORM_WINDOWS

#include <Windows.h>

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

	IwEngine::WindowOptions options{
		1280,
		720,
		IwEngine::NORMAL,
		true,
	};

	IwEngine::Application* app = CreateApplication();
	int status;
	if (status = app->Initilize(options)) {
		LOG_ERROR
			<< "Application initilization failed with error code "
			<< status;
	}

	else {
		app->Run();
	}

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
