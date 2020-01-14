#pragma once

#include "iw/renderer/PipelineParam.h"
#include <string>

namespace IW {
namespace RenderAPI {
	class IWRENDERER_API GLPipelineParam
		: public IPipelineParam
	{
	private:
		unsigned  m_location;
		unsigned& m_textureCount;

		UniformType m_type;
		unsigned  m_typeSize;
		unsigned  m_stride;
		unsigned  m_count;
		std::string m_name;

	public:
		GLPipelineParam(
			unsigned location,
			unsigned& textureCount,
			UniformType type,
			unsigned typeSize,
			unsigned stride,
			unsigned count,
			std::string name);

		// Stride is 0 if there is only a single element

		UniformType Type()     const override;
		unsigned    TypeSize() const override;
		unsigned    Stride()   const override;
		unsigned    Count()    const override;
		const std::string& Name() const override;

		void SetAsBool(
			bool value) override;

		void SetAsBools(
			const void* values,
			unsigned count,
			unsigned stride) override;

		void SetAsInt(
			int value) override;

		void SetAsInts(
			const void* values,
			unsigned count,
			unsigned stride) override;

		void SetAsUInt(
			unsigned int value) override;

		void SetAsUInts(
			const void* values,
			unsigned count,
			unsigned stride) override;

		void SetAsFloat(
			float value) override;

		void SetAsFloats(
			const void* values,
			unsigned count,
			unsigned stride) override;

		void SetAsDouble(
			double value) override;

		void SetAsDoubles(
			const void* values,
			unsigned count,
			unsigned stride) override;

		void SetAsMat2(
			const iw::matrix2& matrix) override;

		void SetAsMat3(
			const iw::matrix3& matrix) override;

		void SetAsMat4(
			const iw::matrix4& matrix) override;

		void SetAsTexture(
			const ITexture* texture,
			int index) override;
	};
}

	using namespace RenderAPI;
}
