#pragma once

#include "IwRenderer.h"
#include "iw/renderer/PipelineParamType.h"

namespace iw {
namespace RenderAPI {
	template<
		typename _i,
		//typename _l,
		typename _e>
	class Translator {
	public:
		IWRENDERER_API
		virtual _e Translate(
			CullFace cull) = 0;

		IWRENDERER_API
		virtual _i Translate(
			TextureType textureType) = 0;

		IWRENDERER_API
		virtual _e Translate(
			TextureFormatType textureFormatType) = 0;

		IWRENDERER_API
		virtual _i Translate(
			TextureFormat textureFormat,
			TextureFormatType textureFormatType) = 0;

		IWRENDERER_API
		virtual _i Translate(
			TextureFormat textureFormat) = 0;

		IWRENDERER_API
		virtual _i Translate(
			TextureWrap textureWrap) = 0;

		IWRENDERER_API
		virtual _i Translate(
			TextureFilter textureFilter) = 0;

		IWRENDERER_API
		virtual _i Translate(
			TextureMipmapFilter textureMipmapFilter) = 0;

		// Uniform type conversion
		IWRENDERER_API
		virtual _i Translate(
			_e glenum,
			unsigned& count) = 0;

		IWRENDERER_API
		virtual _i Translate(
			BufferType bufferType) = 0;

		IWRENDERER_API
		virtual _i Translate(
			BufferIOType bufferIOType) = 0;

		IWRENDERER_API
		static Translator& Instance();
	};
}

	using namespace RenderAPI;
}
