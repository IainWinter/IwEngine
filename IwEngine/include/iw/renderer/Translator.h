#pragma once

#include "IwRenderer.h"

namespace IW {
namespace RenderAPI {
	template<
		typename _i,
		//typename _l,
		typename _e>
	class Translator {
	public:	
		virtual _e Translate(
			TextureFormatType textureFormatType) = 0;

		virtual _i Translate(
			TextureFormat textureFormat,
			TextureFormatType textureFormatType) = 0;

		virtual _i Translate(
			TextureWrap textureWrap) = 0;

		IWRENDERER_API
		static Translator& Instance();
	};
}

	using namespace RenderAPI;
}
