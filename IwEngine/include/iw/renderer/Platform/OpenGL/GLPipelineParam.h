#pragma once

#include "iw/renderer/PipelineParam.h"
#include <string>

namespace IW {
namespace RenderAPI {
	class GLPipelineParam
		: public IPipelineParam
	{
	private:
		unsigned  m_location;
		unsigned& m_textureCount;

		std::string m_name;
		UniformType m_type;
		unsigned m_typeSize;
		unsigned m_stride;
		unsigned m_count;

	public:
		IWRENDERER_API
		GLPipelineParam(
			unsigned location,
			unsigned& textureCount,
			std::string name,
			UniformType type,
			unsigned typeSize,
			unsigned stride,
			unsigned count);

		// Stride is 0 if there is only a single element

		IWRENDERER_API
		const std::string& Name() const override;

		IWRENDERER_API
		UniformType Type() const override;

		IWRENDERER_API
		unsigned TypeSize() const override;

		IWRENDERER_API
		unsigned Stride() const override;

		IWRENDERER_API
		unsigned Count() const override;

		IWRENDERER_API
		void SetAsBool(
			bool value) override;

		IWRENDERER_API
		void SetAsBools(
			const void* values,
			unsigned count,
			unsigned stride) override;

		IWRENDERER_API
		void SetAsInt(
			int value) override;

		IWRENDERER_API
		void SetAsInts(
			const void* values,
			unsigned count,
			unsigned stride) override;

		IWRENDERER_API
		void SetAsUInt(
			unsigned int value) override;

		IWRENDERER_API
		void SetAsUInts(
			const void* values,
			unsigned count,
			unsigned stride) override;

		IWRENDERER_API
		void SetAsFloat(
			float value) override;

		IWRENDERER_API
		void SetAsFloats(
			const void* values,
			unsigned count,
			unsigned stride) override;

		IWRENDERER_API
		void SetAsDouble(
			double value) override;

		IWRENDERER_API
		void SetAsDoubles(
			const void* values,
			unsigned count,
			unsigned stride) override;

		IWRENDERER_API
		void SetAsMat2(
			const iw::matrix2& matrix) override;

		IWRENDERER_API
		void SetAsMat3(
			const iw::matrix3& matrix) override;

		IWRENDERER_API
		void SetAsMat4(
			const iw::matrix4& matrix) override;

		IWRENDERER_API
		void SetAsTexture(
			const ITexture* texture,
			int index) override;
	};
}

	using namespace RenderAPI;
}
