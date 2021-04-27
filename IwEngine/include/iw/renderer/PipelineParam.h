#pragma once

#include "IwRenderer.h"
#include "Texture.h"
#include "PipelineParamType.h"
//#include "iw/math/matrix2.h"
//#include "iw/math/matrix3.h"
//#include "iw/math/matrix4.h"

#include "glm/mat2x2.hpp"
#include "glm/mat3x3.hpp"
#include "glm/mat4x4.hpp"

#include "iw/log/logger.h"

namespace iw {
namespace RenderAPI {
	class IPipelineParam {
	public:
		virtual ~IPipelineParam() = default;

		IWRENDERER_API
		virtual const std::string& Name() const = 0;

		IWRENDERER_API
		virtual unsigned Location() const = 0;

		IWRENDERER_API
		virtual UniformType Type() const = 0;

		IWRENDERER_API
		virtual unsigned TypeSize() const = 0;

		IWRENDERER_API
		virtual unsigned Stride() const = 0;

		IWRENDERER_API
		virtual unsigned Count() const = 0;

#define SET_AS(t, n)               \
	IWRENDERER_API                \
	virtual void SetAs##n(        \
		t value) = 0;            \
                                   \
	IWRENDERER_API                \
	virtual void SetAs##n##s(     \
		const void* values,	     \
		unsigned stride,         \
		unsigned count = 1) = 0; \


	SET_AS(bool,     Bool)
	SET_AS(int,      Int)
	SET_AS(unsigned, UInt)
	SET_AS(float,    Float)
	SET_AS(double,   Double)

#undef SET_AS

		// does set as floats work for this? if so can make them just call that from here

		IWRENDERER_API
		virtual void SetAsMat2s(
			const void* matrix,
			int count = 1) = 0;

		IWRENDERER_API
		virtual void SetAsMat3s(
			const void* matrix,
			int count = 1) = 0;

		IWRENDERER_API
		virtual void SetAsMat4s(
			const void* matrix,
			int count = 1) = 0;

		IWRENDERER_API
		virtual void SetAsMat2(
			const glm::mat2& matrix) = 0;

		IWRENDERER_API
		virtual void SetAsMat3(
			const glm::mat3& matrix) = 0;

		IWRENDERER_API
		virtual void SetAsMat4(
			const glm::mat4& matrix) = 0;

		// samplers
		IWRENDERER_API
		virtual void SetAsTexture(
			const ITexture* texture,
			int index = -1) = 0;

		// image units
		IWRENDERER_API
		virtual void SetAsImage(
			const ITexture* texture,
			int index       = -1,
			unsigned mipmap =  0) = 0;
	};
}
	using namespace RenderAPI;

}
