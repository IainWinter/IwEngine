#pragma once

#include "IwGraphics.h"
#include "iw/renderer/FrameBuffer.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"
#include "Texture.h"

namespace iw {
namespace Graphics {
	struct RenderTarget {
	private:
		unsigned m_width;
		unsigned m_height;
		bool m_noColor;
		std::vector<iw::ref<Texture>> m_textures;

		IFrameBuffer* m_handle;

	public:
		IWGRAPHICS_API
		RenderTarget(
			bool noColor = false);

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void Use(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void ReadPixels(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void AddTexture(
			iw::ref<Texture> texture);

		IWGRAPHICS_API
		unsigned Width() const;

		IWGRAPHICS_API
		unsigned Height() const;

		IWGRAPHICS_API
		void Resize(
			unsigned width,
			unsigned height);

		IWGRAPHICS_API
		const iw::ref<Texture>& Tex(
			unsigned index) const;

		IWGRAPHICS_API
		IFrameBuffer* Handle() const;

		IWGRAPHICS_API
		bool IsInitialized() const;
	};
}

	using namespace Graphics;
}
