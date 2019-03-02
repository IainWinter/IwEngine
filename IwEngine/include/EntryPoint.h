#pragma once

#include "Application.h"

extern IwEngine::Application* CreateApplication();

#ifdef IW_PLATFORM_WINDOWS

int CALLBACK WinMain(
	HINSTANCE handle,
	HINSTANCE prevhandle,
	LPSTR cmdline,
	int cmdshow)
{
	IwEngine::Application* app = CreateApplication();
	app->Initilize();
	app->Start();
	app->Run();
	app->Stop();
	app->Destroy();
	delete app;
}

#endif
