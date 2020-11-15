#pragma once

#include "iw/engine/Layer.h"

class CS420Chess
	: public iw::Layer
{
private:
	iw::Prefab m_peice;

public:
	CS420Chess();

	int Initialize();
	//void PostUpdate();
};
