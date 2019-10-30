#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/util/tuple/foreach.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include <vector>
#include <string>

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Material {
	private:
		enum MaterialPropertyType {
			BOOL,
			INT,
			UINT,
			FLOAT,
			DOUBLE
		};

		struct MaterialProperty {
			const char* Name;

			size_t Size;
			size_t Stride;

			bool IsSample;
			MaterialPropertyType Type;

			void* Data;

			~MaterialProperty() {
				delete[] Name;
				free(Data);
			}
		};

	public:
		iwu::ref<IW::IPipeline> Pipeline;

	private:
		std::vector<MaterialProperty> m_scalars;

	public:
		Material();

		Material(
			iwu::ref<IW::IPipeline>& pipline);

		~Material();

		void SetBool(
			const char* name,
			bool value);

		void SetBools(
			const char* name,
			bool* values,
			size_t size,
			size_t stride);

		void SetInt(
			const char* name,
			int value);

		void SetInts(
			const char* name,
			int* values,
			size_t size,
			size_t stride);

		void SetUInt(
			const char* name,
			unsigned int value);

		void SetUInts(
			const char* name,
			unsigned int* values,
			size_t size,
			size_t stride);

		void SetFloat(
			const char* name,
			float value);

		void SetFloats(
			const char* name,
			float* values,
			size_t size,
			size_t stride);

		void SetDouble(
			const char* name,
			double value);

		void SetDouble(
			const char* name,
			double* values,
			size_t size,
			size_t stride);

		bool& GetBool(
			const char* name);

		bool*& GetBools(
			const char* name);

		/*void SetTexture(
			const char* name,
			IW::Testure texture);*/

		void Use(
			const iwu::ref<IW::IDevice>& device) const;
	private:
		template<
			typename _t>
		typename std::vector<_t>::iterator FindName(
			std::string name,
			std::vector<_t>& vector)
		{
			auto itr = vector.begin();
			for (; itr != vector.end(); itr++) {
				if (itr->Name == name) {
					break;
				}
			}

			return itr;
		}
	};
}
}
