#pragma once

#include "IwRenderer.h"
#include "Buffer.h"

namespace iw {
namespace RenderAPI {
	class IUniformBuffer
		: public virtual IBuffer
	{
	public:
		IWRENDERER_API
		virtual ~IUniformBuffer() = default;

		IWRENDERER_API
		virtual void BindMyBase() const = 0;

		IWRENDERER_API
		virtual unsigned MyBase() const = 0;

		IWRENDERER_API
		virtual void SetMyBase(
			unsigned base) = 0;

	protected:
		IUniformBuffer() = default;
	};
}

	using namespace RenderAPI;
}
