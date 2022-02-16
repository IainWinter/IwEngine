#include "Systems/Recording_System.h"

void Recording_System::RecordFrame(
	const iw::ref<iw::Texture>& frame)
{
	int x_cur = m_player.Find<iw::Transform>()->Position.x;
	int y_cur = m_player.Find<iw::Transform>()->Position.y;

	int x_min = x_cur - m_frameSize / 2;
	int y_min = y_cur - m_frameSize / 2;
	int x_max = x_cur + m_frameSize / 2;
	int y_max = y_cur + m_frameSize / 2;

	for (int x = 0, xf = x_min; xf < x_max; x++, xf++)
	for (int y = 0, yf = y_min; yf < y_max; y++, yf++)
	{
		int fi = xf + yf * frame->m_width;
		int  i = x + y * m_frameCount;
		m_recording.GetSubTexture(m_frameToDraw)->Colors32()[i] = frame->Colors32()[fi];
	}

	// oldest frame will get replaced
	m_frameToDraw = (m_frameToDraw + 1) % m_frameCount;
}
