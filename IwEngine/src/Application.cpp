#include "Core/Application.h"
#include <iostream>

IwEngine::Application::Application() {
	std::cout << "Constructing application" << std::endl;
}

IwEngine::Application::~Application() {
	std::cout << "Deconstructing application" << std::endl;
}

void IwEngine::Application::Run() {
	std::cout << "Running application" << std::endl;
}