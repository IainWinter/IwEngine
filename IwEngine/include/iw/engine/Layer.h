#pragma once

#include "Core.h"
#include "Event.h"

namespace IwEngine {
	class IWENGINE_API Layer {
	private:
		const char* m_name;

	public:
		Layer(
			const char* name);

		virtual ~Layer();

		virtual int  Initilize();
		virtual void Destroy();
		virtual void Update();

		virtual void OnEvent(
			Event& e);

		inline const char* Name() {
			return m_name;
		}
	};
}
