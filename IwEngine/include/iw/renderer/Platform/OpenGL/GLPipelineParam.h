#pragma once

#include "iw/renderer/PipelineParam.h"
#include <string>

namespace iw {
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
		unsigned Location() const override;

		IWRENDERER_API
		UniformType Type() const override;

		IWRENDERER_API
		unsigned TypeSize() const override;

		IWRENDERER_API
		unsigned Stride() const override;

		IWRENDERER_API
		unsigned Count() const override;

#define SET_AS(t, n)                    \
	IWRENDERER_API                     \
	void SetAs##n(                     \
		t value) override;            \
                                        \
	IWRENDERER_API                     \
	void SetAs##n##s(                  \
		const void* values,	          \
		unsigned stride,              \
		unsigned count = 1) override; \

	SET_AS(bool,     Bool)
	SET_AS(int,      Int)
	SET_AS(unsigned, UInt)
	SET_AS(float,    Float)
	SET_AS(double,   Double)

#undef SET_AS

		IWRENDERER_API
		void SetAsMat2s(
			const void* matrix,
			int count = 1) override;

		IWRENDERER_API
		void SetAsMat3s(
			const void* matrix,
			int count = 1) override;

		IWRENDERER_API
		void SetAsMat4s(
			const void* matrix,
			int count = 1) override;

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
