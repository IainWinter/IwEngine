#pragma once

#include "iw/engine/System.h"
#include "plugins/iw/Sand/Engine/SandLayer.h"
#include "iw/graphics/TextureAtlas.h"
#include "Events.h"

struct Recording_System : iw::SystemBase
{
	int m_frameCount;
	int m_frameSize; // always a square
	int m_frameToDraw;
	iw::TextureAtlas m_recording;

	iw::SandLayer* m_sand;
	entity m_player;

	float m_last_x;
	float m_last_y;

	bool m_record;

	Recording_System(
		iw::SandLayer* sand
	)
		: iw::SystemBase ("Recording system test")
		, m_sand         (sand)
		, m_frameCount   (150)
		, m_frameSize    (100)
		, m_frameToDraw  (0)
		, m_last_x       (0.f)
		, m_last_y       (0.f)
		, m_record       (true) // only because popping a system changes the state in a bad way, need to get the movie before it gets removed, but if the layer pops the system, then it doesnt get cleaned up
	{
		m_recording = iw::TextureAtlas(1, m_frameCount, REF<iw::Texture>(m_frameSize, m_frameSize * m_frameCount));
		m_recording.m_texture->m_filter = iw::NEAREST;
		m_recording.m_texture->CreateColors();
	}

	void OnPush() override
	{
		m_sand->m_render->m_afterRender = [=](
			const iw::ref<iw::Texture>& frame)
		{
			RecordFrame(frame);
		};
	}

	void OnPop() override
	{
		m_sand->m_render->m_afterRender = {};
	}

	bool On(iw::ActionEvent& e)
	{
		if (e.Action == CREATED_PLAYER)
		{
			m_player = e.as<CreatedPlayer_Event>().PlayerEntity;
		}

		return false;
	}

	void RecordFrame(const iw::ref<iw::Texture>& frame);
};
