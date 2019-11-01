#pragma once

#include "IwRenderer.h"

namespace IW {
	inline namespace RenderAPI {
		class IWRENDERER_API ITexture {
		public:
			virtual ~ITexture() {}

		protected:
			ITexture() {}
		};
	}
}
