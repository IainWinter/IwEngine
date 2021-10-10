#pragma once

#include "Application.h"
#include "iw/log/logger.h"

#ifdef IW_PLATFORM_WINDOWS

#include <Windows.h>

int WINAPI WinMain(
	HINSTANCE handle,
	HINSTANCE prevhandle,
	LPSTR cmdline,
	int cmdshow)
{
	iw::InitOptions options;

	iw::Application* app = CreateApplication(options);

	if (int status = app->Initialize(options)) {
		LOG_ERROR
			<< "Application initilization failed with error code "
			<< status;
	}

	else {
		app->Run();
	}

	delete app;
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
