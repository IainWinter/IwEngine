#pragma once

#include "iw/renderer/PipelineParam.h"

namespace IW {
inline namespace RenderAPI {
	class IWRENDERER_API GLPipelineParam
		: public IPipelineParam
	{
	private:
		unsigned int m_location;
	public:
		GLPipelineParam(int location);

		// Stride is 0 if there is only a single element

		void SetAsBool(
			bool value) override;

		void SetAsBools(
			const void* values,
			size_t count,
			size_t stride) override;

		void SetAsInt(
			int value) override;

		void SetAsInts(
			const void* values,
			size_t count,
			size_t stride) override;

		void SetAsUInt(
			unsigned int value) override;

		void SetAsUInts(
			const void* values,
			size_t count,
			size_t stride) override;

		void SetAsFloat(
			float value) override;

		void SetAsFloats(
			const void* values,
			size_t count,
			size_t stride) override;

		void SetAsDouble(
			double value) override;

		void SetAsDoubles(
			const void* values,
			size_t count,
			size_t stride) override;

		void SetAsMat2(
			const iwm::matrix2& matrix) override;

		void SetAsMat3(
			const iwm::matrix3& matrix) override;

		void SetAsMat4(
			const iwm::matrix4& matrix) override;
	};
}
}
