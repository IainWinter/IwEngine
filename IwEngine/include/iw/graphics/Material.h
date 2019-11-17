#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "Texture.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/util/tuple/foreach.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include <vector>
#include <tuple>

namespace IW {
inline namespace Graphics {
	struct IWGRAPHICS_API Material {
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
		iw::ref<IW::IPipeline> Pipeline;

	private:
		std::vector<MaterialProperty> m_properties;

	public:
		Material();

		Material(
			iw::ref<IW::IPipeline>& pipline);

		Material(
			Material&&) noexcept = default;

		Material(
			const Material& copy);

		~Material();

		Material& operator=(
			Material&&) noexcept = default;

		Material& operator=(
			const Material& copy);

		void SetBool(
			const char* name,
			bool value);

		// Stride is 0 if there is only a single element

		void SetBools(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		void SetInt(
			const char* name,
			int value);

		void SetInts(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		void SetUInt(
			const char* name,
			unsigned int value);

		void SetUInts(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		void SetFloat(
			const char* name,
			float value);

		void SetFloats(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		void SetDouble(
			const char* name,
			double value);

		void SetDoubles(
			const char* name,
			void* values,
			size_t count,
			size_t stride = 0);

		void SetTexture(
			const char* name,
			IW::Texture* texture);

		void SetTexture(
			const char* name,
			const iw::ref<IW::Texture>& texture);

		bool* GetBool(
			const char* name);

		std::tuple<bool*, size_t> GetBools(
			const char* name);

		int* GetInt(
			const char* name);

		std::tuple<int*, size_t> GetInts(
			const char* name);

		unsigned int* GetUInt(
			const char* name);

		std::tuple<unsigned int*, size_t> GetUInts(
			const char* name);

		float* GetFloat(
			const char* name);

		std::tuple<float*, size_t> GetFloats(
			const char* name);

		double* GetDouble(
			const char* name);

		std::tuple<double*, size_t> GetDoubles(
			const char* name);

		IW::Texture* GetTexture(
			const char* name);

		void Use(
			const iw::ref<IW::IDevice>& device) const;
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
}
