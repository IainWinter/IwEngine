#include "iw/graphics/RenderTarget.h"

namespace IW {
	RenderTarget::RenderTarget(
		int width, 
		int height,
		std::initializer_list<TextureFormat> formats,
		std::initializer_list<TextureFormatType> types)
		: Width(width)
		, Height(height)
		, Frame(nullptr)
	{
		if (formats.size() != types.size()) return;

		TextureCount = formats.size();
		Textures = new Texture[TextureCount];

		auto fitr = formats.begin();
		auto titr = types  .begin();
		for (size_t i = 0; i < TextureCount; i++, fitr++, titr++) {
			Textures[i] = {
				Width,
				Height,
				*fitr,
				*titr,
				nullptr
			};
		}
	}

	void RenderTarget::Initialize(
		const iw::ref<IDevice>& device)
	{
		Frame = device->CreateFrameBuffer();
		
		for (size_t i = 0; i < TextureCount; i++) {
			Textures[i].Initialize(device);
			device->AttachTextureToFrameBuffer(Frame, Textures[i].Handle);
		}
	}

	void RenderTarget::Use(
		const iw::ref<IDevice>& device)
	{
		device->SetFrameBuffer(Frame);
		device->SetViewport(Width, Height);
	}
}
