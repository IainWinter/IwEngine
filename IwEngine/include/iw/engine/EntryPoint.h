#pragma once

#include "Application.h"
#include "iw/log/logger.h"

extern iw::Application* CreateApplication(
	iw::InitOptions& options);

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

// If in editor with macro

extern "C" __declspec(dllexport)
iw::Application* GetApplicationForEditor();

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
