#pragma once

#include "Device.h"
#include "iw/input/Events/InputEvent.h"

namespace IwInput {
	class MouseBase
		: public Device
	{
	public:
		MouseBase(
			InputCallback& callback)
			: Device(callback)
		{}

		virtual ~MouseBase() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		virtual void SetDimensions(
			float width,
			float height) = 0;

		virtual float Width()  = 0;
		virtual float Height() = 0;
	};

	class Mouse
		: public MouseBase
	{
	private:
		float m_width;
		float m_height;
	protected:
		static Translation CreateTranslation();

	public:
		Mouse(
			InputCallback& callback)
			: MouseBase(callback)
		{}

		virtual ~Mouse() {}

		virtual void HandleEvent(
			OsEvent& event) = 0;

		inline void SetDimensions(
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

		static InputName Translate(
			unsigned int oskey);

		static Mouse* Create(
			InputCallback& callback);
	};
}
