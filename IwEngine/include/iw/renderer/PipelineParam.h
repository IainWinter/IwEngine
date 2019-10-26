#pragma once

#include "IwRenderer.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include "iw/math/matrix2.h"
#include "iw/math/matrix3.h"
#include "iw/math/matrix4.h"
#include "iw/log/logger.h"

namespace IW {
inline namespace RenderAPI {
	enum PipelineParamType {
		FLOAT,
		VEC2,
		VEC3,
		VEC4,
		MAT2,
		MAT3,
		MAT4,
		NOTYPE
	};

	template<
		typename _t>
	PipelineParamType GetTypeOfParam() {
		const size_t f  = typeid(float).hash_code();
		const size_t v2 = typeid(iwm::vector2).hash_code();
		const size_t v3 = typeid(iwm::vector3).hash_code();
		const size_t v4 = typeid(iwm::vector4).hash_code();
		const size_t m2 = typeid(iwm::matrix2).hash_code();
		const size_t m3 = typeid(iwm::matrix3).hash_code();
		const size_t m4 = typeid(iwm::matrix4).hash_code();

		size_t hash = typeid(_t).hash_code();

		if      (hash == f)  { return FLOAT; }
		else if (hash == v2) { return VEC2; }
		else if (hash == v3) { return VEC3; }
		else if (hash == v4) { return VEC4; }
		else if (hash == m2) { return MAT2; }
		else if (hash == m3) { return MAT3; }
		else if (hash == m4) { return MAT4; }

		LOG_WARNING << "Nonvalid pipeline param type!";

		return NOTYPE;
	}

	//leak that state

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
}
