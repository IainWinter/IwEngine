#include "iw/graphics/RenderTarget.h"

namespace iw {
namespace Graphics {
	RenderTarget::RenderTarget()
		: m_width(0)
		, m_height(0)
		, m_frameBuffer(nullptr)
	{}

	RenderTarget::~RenderTarget()
	{
		delete m_frameBuffer;
	}

	void RenderTarget::Initialize(
		const iw::ref<IDevice>& device)
	{
		m_frameBuffer = device->CreateFrameBuffer();

		std::vector<unsigned> colors;

		for (iw::ref<Texture>& texture : m_textures) {
			texture->Initialize(device);
			bool isColor = m_frameBuffer->AttachTexture(texture->Handle());

			if (isColor) {
				colors.push_back(colors.size());
			}
		}

		m_frameBuffer->SetDrawable(colors);
	}

	void RenderTarget::Use(
		const iw::ref<IDevice>& device)
	{
		device->SetFrameBuffer(m_frameBuffer);
		device->SetViewport(m_width, m_height);
	}

	void RenderTarget::ReadPixels() {
		m_frameBuffer->ReadPixels();
	}

	void RenderTarget::WritePixels() {
		m_frameBuffer->WritePixels();
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

		for(iw::ref<iw::Texture>& tex : m_textures) {
			tex->SetPixels(m_width, m_height);
		}
	}

	const iw::ref<Texture>& RenderTarget::Tex(
		unsigned index) const
	{
		return m_textures.at(index);
	}

	IFrameBuffer* RenderTarget::Handle() const {
		return m_frameBuffer;
	}

	bool RenderTarget::IsInitialized() const {
		return !!m_frameBuffer;
	}
}
}
