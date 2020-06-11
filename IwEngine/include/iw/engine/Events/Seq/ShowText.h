#pragma once

#include "iw/engine/Events/Seq/ShowText.h"
#include "iw/common/Components/Transform.h"
#include "iw/entity/Entity.h"
#include "iw/graphics/Font.h"

namespace iw {
namespace Engine {
	struct ShowText
		: EventTask
	{
	private:
		iw::Entity entity;
		iw::ref<iw::Font> font;
		std::string text;
		float timeout;

	public:
		ShowText(
			iw::Entity entity,
			iw::ref<iw::Font> font,
			std::string text,
			float timeout)
			: text(text)
			, timeout(timeout)
		{
			iw::Mesh* mesh = entity.Find<iw::Mesh>();
			*mesh = font->GenerateMesh(text, 0.01f, 0.02f);
		}

		void reset() override {
			
		}

		bool update() override {


			return true;
		}
	};
}

	using namespace Engine;
}
