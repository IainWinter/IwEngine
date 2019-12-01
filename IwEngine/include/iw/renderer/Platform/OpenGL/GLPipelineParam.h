#pragma once

#include "iw/renderer/PipelineParam.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLPipelineParam
		: public IPipelineParam
	{
	private:
		unsigned int m_location;
		unsigned int& m_textureCount;

	public:
		GLPipelineParam(
			int location,
			unsigned int& textureCount);

		// Stride is 0 if there is only a single element

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
			const ITexture* texture) override;
	};
}
}
