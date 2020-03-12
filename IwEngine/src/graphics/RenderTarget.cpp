#include "iw/graphics/RenderTarget.h"
#include <assert.h>

namespace iw {
namespace Graphics {
	RenderTarget::RenderTarget(
		int width,
		int height)
		: m_width(width)
		, m_height(height)
		, m_handle(nullptr)
	{}

	void RenderTarget::Initialize(
		const iw::ref<IDevice>& device)
	{
		m_handle = device->CreateFrameBuffer();

		for (iw::ref<Texture>& texture : m_textures) {
			texture->Initialize(device);
			device->AttachTextureToFrameBuffer(m_handle, texture->Handle());
		}
	}

	void RenderTarget::Use(
		const iw::ref<IDevice>& device)
	{
		device->SetFrameBuffer(m_handle);
		device->SetViewport(m_width, m_height);
	}

	void RenderTarget::AddTexture(
		iw::ref<Texture> texture)
	{
		m_textures.push_back(texture);
	}

	int RenderTarget::Width() const {
		return m_width;
	}

	int RenderTarget::Height() const {
		return m_height;
	}

	const iw::ref<Texture>& RenderTarget::Tex(
		int index) const
	{
		return m_textures.at(index);
	}

	IFrameBuffer* RenderTarget::Handle() const {
		return m_handle;
	}
}
}
