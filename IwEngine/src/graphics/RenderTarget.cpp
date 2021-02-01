#include "iw/graphics/RenderTarget.h"

namespace iw {
namespace Graphics {
	RenderTarget::RenderTarget(
		bool noColor)
		: m_noColor(noColor)
		, m_handle(nullptr)
	{}

	void RenderTarget::Initialize(
		const iw::ref<IDevice>& device)
	{
		m_handle = device->CreateFrameBuffer(m_noColor);

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

	void RenderTarget::ReadPixels(
		const iw::ref<IDevice>& device)
	{
		device->ReadPixelsFromFrameBuffer(m_handle);
	}

	void RenderTarget::AddTexture(
		iw::ref<Texture> texture)
	{
		if (m_textures.size() == 0) {
			Resize(texture->Width(), texture->Height());
		}

		if (   Width()  == texture->Width()
			&& Height() == texture->Height())
		{
			m_textures.push_back(texture);
		}

		else {
			LOG_WARNING << "Tried to add texture with different dim that target!"; // put w, w, h, h
		}
	}

	unsigned RenderTarget::Width() const {
		return m_width;
	}

	unsigned RenderTarget::Height() const {
		return m_height;
	}

	void RenderTarget::Resize(
		unsigned width,
		unsigned height)
	{
		m_width  = width;
		m_height = height;
	}

	const iw::ref<Texture>& RenderTarget::Tex(
		unsigned index) const
	{
		return m_textures.at(index);
	}

	IFrameBuffer* RenderTarget::Handle() const {
		return m_handle;
	}

	bool RenderTarget::IsInitialized() const {
		return !!m_handle;
	}
}
}
