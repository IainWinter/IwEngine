#pragma once

#include "Application.h"
#include "log.h"

extern IwEngine::Application* CreateApplication();

#ifdef IW_PLATFORM_WINDOWS

int CALLBACK WinMain(
	HINSTANCE handle,
	HINSTANCE prevhandle,
	LPSTR cmdline,
	int cmdshow)
{
	IwEngine::Application* app = CreateApplication();
	int status = app->Initilize();

	if (status == 0) {
		app->Start();
	}

	else {
		//std::cout << "Application Initilization failed!" << std::endl;
	}

	delete app;
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
