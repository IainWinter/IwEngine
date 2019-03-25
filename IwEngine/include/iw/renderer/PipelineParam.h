#pragma once

#include "IwRenderer.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include "iw/math/matrix2.h"
#include "iw/math/matrix3.h"
#include "iw/math/matrix4.h"

namespace IwRenderer {
	class IWRENDERER_API IPipelineParam {
	public:
		virtual ~IPipelineParam() {}

		virtual void SetAsFloat(
			const float& val) = 0;

		virtual void SetAsVec2(
			const iwm::vector2& vec) = 0;

		virtual void SetAsVec3(
			const iwm::vector3& vec) = 0;

		virtual void SetAsVec4(
			const iwm::vector4& vec) = 0;

		virtual void SetAsMat2(
			const iwm::matrix2& matrix) = 0;

		virtual void SetAsMat3(
			const iwm::matrix3& matrix) = 0;

		virtual void SetAsMat4(
			const iwm::matrix4& matrix) = 0;
	};
}
