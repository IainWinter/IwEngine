#pragma once 

#include "iw/engine/EntryPoint.h"

class App : public iw::Application {
public:
	App();
	int Initialize(iw::InitOptions& options) override;
};
