#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

namespace IwInput {
	class IMouse
		: public IDevice
	{
	public:
		virtual ~IMouse() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		virtual void SetDimensions(
			float width,
			float height) = 0;

		virtual float Width()  = 0;
		virtual float Height() = 0;
	};

	class Mouse
		: public Device
	{
	private:
		float m_width;
		float m_height;

	public:
		virtual ~Mouse() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		virtual void SetDimensions(
			float width,
			float height)
		{
			m_width  = width;
			m_height = height;
		}

		inline float Width() {
			return m_width;
		}

		inline float Height() {
			return m_height;
		}
	};
}
