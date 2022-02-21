#include "Systems/Recording_System.h"

void Recording_System::RecordFrame(
	const iw::ref<iw::Texture>& frame)
{
	if (m_player.is_alive())
	{
		m_last_x = m_player.get<iw::Transform>().Position.x;
		m_last_y = m_player.get<iw::Transform>().Position.y;
	}

	int x_cur = floor(      m_last_x);
	int y_cur = floor(400 - m_last_y);

	int x_min = x_cur - m_frameSize / 2;
	int y_min = y_cur - m_frameSize / 2;
	int x_max = x_cur + m_frameSize / 2;
	int y_max = y_cur + m_frameSize / 2;

	int i_offset = m_frameSize*m_frameSize * m_frameToDraw;

	for (int y = 0, yf = y_min; yf < y_max; y++, yf++)
	for (int x = 0, xf = x_min; xf < x_max; x++, xf++)
	{
		int fi = xf + yf * frame->m_width;
		int  i = x + y * m_frameSize;
		m_recording.m_texture->Colors32()[i + i_offset] = frame->Colors32()[fi];
	}

	// oldest frame will get replaced
	m_frameToDraw = (m_frameToDraw + 1) % m_frameCount;
}
