#pragma once

#include "iw/engine/System.h"
#include "iw/graphics/Texture.h"

#include "plugins/iw/Sand/SandWorker.h"

// should go into sand library
struct Tile {
	std::vector<std::pair<int, int>> Positions;
	float X = 0;
	float Y = 0;
};

struct HeatField {
	float Tempeture;
};

// Handles updating and rendering the sand texture,
//	rendering should be put into a mesh rendering system using the ecs

class SandWorldUpdateSystem : public iw::SystemBase {
private:
	iw::SandWorld& m_world;
	iw::ref<iw::Texture>& m_texture;

	int m_fx,  m_fy,
	    m_fx2, m_fy2; // Camera frustrum

public:
	SandWorldUpdateSystem(iw::SandWorld& world, iw::ref<iw::Texture>& texture)
		: SystemBase("Sand world update")
		, m_world  (world)
		, m_texture(texture)
	{}

	int  Initialize() override;
	void Update()     override;

	void SetCamera(
		int fx,  int fy,
		int fx2, int fy2)
	{
		m_fx  = fx;  m_fy  = fy;
		m_fx2 = fx2; m_fy2 = fy2;
	}
};
