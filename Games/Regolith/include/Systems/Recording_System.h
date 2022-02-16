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
	iw::Entity m_player;

	Recording_System(
		iw::SandLayer* sand
	)
		: iw::SystemBase ("Recording system test")
		, m_sand         (sand)
		, m_frameCount   (10)
		, m_frameSize    (50)
		, m_frameToDraw  (0)
	{
		// 50x50x10

		m_recording = iw::TextureAtlas(m_frameSize * m_frameCount, m_frameSize * m_frameCount);
		m_recording.m_filter = iw::NEAREST;
		m_recording.GenTexBounds(m_frameCount, m_frameCount);

		m_sand->m_render->m_afterRender = [=](
			const iw::ref<iw::Texture>& frame)
		{
			RecordFrame(frame);
		};
	}

	bool On(iw::ActionEvent& e)
	{
		if (e.Action == CREATED_PLAYER)
		{
			m_player = e.as<CreatedPlayer_Event>().PlayerEntity;
		}
	}

	void RecordFrame(const iw::ref<iw::Texture>& frame);
};
