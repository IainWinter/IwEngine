#pragma once

#include "IwRenderer.h"
#include "iw/renderer/PipelineParamType.h"

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

		virtual _i Translate(
			_e glenum,
			unsigned& count) = 0;

		IWRENDERER_API
		static Translator& Instance();
	};
}

	using namespace RenderAPI;
}