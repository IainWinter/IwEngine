#pragma once

#include "iw/engine/System.h"
#include "../SandWorld.h"

IW_PLUGIN_SAND_BEGIN

class SandWorldRenderSystem
	: public SystemBase
{
public:
	int m_fx,  m_fy,
	    m_fx2, m_fy2; // Camera frustrum
private:
	SandWorld* m_world;
	int m_worldWidth;
	int m_worldHeight;

	Mesh         m_mesh;
	ref<Texture> m_texture;

public:
	SandWorldRenderSystem(
		SandWorld* world,
		int worldWidth,
		int worldHeight
	)
		: SystemBase("Sand World Render")
		, m_world(world)
		, m_worldWidth(worldWidth)
		, m_worldHeight(worldHeight)
	{
		SetCamera(0, 0, 100, 100);
	}

	IW_PLUGIN_SAND_API int  Initialize() override;
	IW_PLUGIN_SAND_API void Update() override;

	void SetCamera(
		int fx,  int fy,
		int fx2, int fy2)
	{
		m_fx  = fx;  m_fy  = fy;
		m_fx2 = fx2; m_fy2 = fy2;
	}

	Mesh& GetSandMesh() {
		return m_mesh;
	}

	ref<Texture>& GetSandTexture() {
		return m_texture;
	}
};

IW_PLUGIN_SAND_END
