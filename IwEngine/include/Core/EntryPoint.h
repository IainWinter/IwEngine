#pragma once

#include "Application.h"

extern IwEngine::Application* CreateApplication();

#ifdef IW_PLATFORM_WINDOWS

int main() {
	IwEngine::Application* app = CreateApplication();
	app->Run();
	delete app;
}

#endif