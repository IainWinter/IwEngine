#pragma once

#include "IwRenderer.h"
#include "Texture.h"
#include "PipelineParamType.h"
#include "iw/math/matrix2.h"
#include "iw/math/matrix3.h"
#include "iw/math/matrix4.h"
#include "iw/log/logger.h"

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API IPipelineParam {
	public:
		virtual ~IPipelineParam() {}

		virtual UniformType Type()        const = 0;
		virtual unsigned    TypeSize()    const = 0;
		virtual unsigned    Stride()      const = 0;
		virtual unsigned    Count()       const = 0;
		virtual const std::string& Name() const = 0;

		virtual void SetAsBool(
			bool value) = 0;

		virtual void SetAsBools(
			const void* values,
			unsigned count,
			unsigned stride = 0) = 0;

		virtual void SetAsInt(
			int value) = 0;

		virtual void SetAsInts(
			const void* values,
			unsigned count,
			unsigned stride = 0) = 0;

		virtual void SetAsUInt(
			unsigned int value) = 0;

		virtual void SetAsUInts(
			const void* values,
			unsigned count,
			unsigned stride = 0) = 0;

		virtual void SetAsFloat(
			float value) = 0;

		virtual void SetAsFloats(
			const void* values,
			unsigned count,
			unsigned stride = 0) = 0;

		virtual void SetAsDouble(
			double value) = 0;

		virtual void SetAsDoubles(
			const void* values,
			unsigned count,
			unsigned stride = 0) = 0;

		virtual void SetAsMat2(
			const iw::matrix2& matrix) = 0;

		virtual void SetAsMat3(
			const iw::matrix3& matrix) = 0;

		virtual void SetAsMat4(
			const iw::matrix4& matrix) = 0;

		virtual void SetAsTexture(
			const ITexture* texture) = 0;
	};
}
	using namespace RenderAPI;

}
