#pragma once

#include "iw/renderer/PipelineParam.h"

namespace IwRenderer {
	class IWRENDERER_API GLPipelineParam
		: public IPipelineParam
	{
	private:
		unsigned int m_location;
	public:
		GLPipelineParam(int location);

		void SetAsFloat(
			const float& val);

		void SetAsVec2(
			const iwm::vector2& vec);

		void SetAsVec3(
			const iwm::vector3& vec);

		void SetAsVec4(
			const iwm::vector4& vec);

		void SetAsMat2(
			const iwm::matrix2& matrix);

		void SetAsMat3(
			const iwm::matrix3& matrix);

		void SetAsMat4(
			const iwm::matrix4& matrix);
	};
}
