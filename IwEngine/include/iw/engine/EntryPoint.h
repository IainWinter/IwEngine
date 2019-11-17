#pragma once

#include "Application.h"
#include "iw/log/logger.h"

extern IW::Application* CreateApplication(
	IW::InitOptions& options);

#ifdef IW_PLATFORM_WINDOWS

#include <Windows.h>

int WINAPI WinMain(
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

	IW::InitOptions options;

	IW::Application* app = CreateApplication(options);
	int status;
	if (status = app->Initialize(options)) {
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
//   Initialize
//       |
//    xStartx
//       |   
//      Run<---
//       |--->^
//       |
//    xStopx
//       |
//    Destroy
//       |
//   Destructor
//
