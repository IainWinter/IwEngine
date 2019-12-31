#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "Texture.h"
#include "Shader.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"
#include "iw/util/tuple/foreach.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include <vector>
#include <tuple>

namespace IW {
namespace Graphics {
	struct Material {
	private:
		enum MaterialPropertyType {
			BOOL,
			INT,
			UINT,
			FLOAT,
			DOUBLE,
			SAMPLE
		};

		struct MaterialProperty {
			char* Name;

			size_t Size;
			size_t Count;
			size_t Stride;

			bool IsSample;
			MaterialPropertyType Type;

			void* Data;
		};

	public:
		iw::ref<Shader> Shader;

	private:
		std::vector<MaterialProperty> m_properties;

	public:
		IWGRAPHICS_API
		Material() = default;

		IWGRAPHICS_API
		Material(
			iw::ref<IW::Shader>& shader);

		IWGRAPHICS_API
		Material(
			Material&&) noexcept = default;

		IWGRAPHICS_API
		Material(
			const Material& copy);

		IWGRAPHICS_API
		~Material();

		IWGRAPHICS_API
		Material& operator=(
			Material&&) noexcept = default;

		IWGRAPHICS_API
		Material& operator=(
			const Material& copy);

		inline void SetShader(
			iw::ref<IW::Shader>& shader)
		{
			Shader = shader;
		}

		IWGRAPHICS_API
		void SetBool(
			const char* name,
			bool value);

		// Stride is 0 if there is only a single element

		IWGRAPHICS_API
		void SetBools(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		IWGRAPHICS_API
		void SetInt(
			const char* name,
			int value);

		IWGRAPHICS_API
		void SetInts(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		IWGRAPHICS_API
		void SetUInt(
			const char* name,
			unsigned int value);

		IWGRAPHICS_API
		void SetUInts(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		IWGRAPHICS_API
		void SetFloat(
			const char* name,
			float value);

		IWGRAPHICS_API
		void SetFloats(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		IWGRAPHICS_API
		void SetDouble(
			const char* name,
			double value);

		IWGRAPHICS_API
		void SetDoubles(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		IWGRAPHICS_API
		void SetTexture(
			const char* name,
			Texture* texture);

		IWGRAPHICS_API
		void SetTexture(
			const char* name,
			const iw::ref<Texture>& texture);

		IWGRAPHICS_API
		bool* GetBool(
			const char* name);

		IWGRAPHICS_API
		std::tuple<bool*, size_t> GetBools(
			const char* name);

		IWGRAPHICS_API
		int* GetInt(
			const char* name);

		IWGRAPHICS_API
		std::tuple<int*, size_t> GetInts(
			const char* name);

		IWGRAPHICS_API
		unsigned int* GetUInt(
			const char* name);

		IWGRAPHICS_API
		std::tuple<unsigned int*, size_t> GetUInts(
			const char* name);

		IWGRAPHICS_API
		float* GetFloat(
			const char* name);

		IWGRAPHICS_API
		std::tuple<float*, size_t> GetFloats(
			const char* name);

		IWGRAPHICS_API
		double* GetDouble(
			const char* name);

		IWGRAPHICS_API
		std::tuple<double*, size_t> GetDoubles(
			const char* name);

		IWGRAPHICS_API
		Texture* GetTexture(
			const char* name);

		IWGRAPHICS_API
		void Use(
			const iw::ref<IDevice>& device) const;
	private:
		std::tuple<void*, size_t> GetData(
			const char* name);

		MaterialProperty* FindProperty(
			const char* name);

		void CreateProperty(
			const char* name,
			void* values,
			size_t count,
			size_t stride,
			bool isSample,
			MaterialPropertyType type,
			size_t typeSize);
	};
}

	using namespace Graphics;
}
