#pragma once

#include "IwGraphics.h"
#include "Color.h"
#include "Texture.h"
#include "Shader.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/ref.h"
#include "iw/util/tuple/foreach.h"
#include "iw/util/memory/linear_allocator.h"
#include "iw/math/vector2.h"
#include "iw/math/vector3.h"
#include "iw/math/vector4.h"
#include <vector>
#include <unordered_map>
#include <tuple>

namespace iw {
namespace Graphics {
	struct Material {
	public:
		iw::ref<Shader> Shader;

	private:
		struct MaterialProperty {
			std::string Name;
			void* Data;
			UniformType Type;
			unsigned TypeSize;
			unsigned Stride;
			unsigned Count;
			bool Active;
		};

		struct TextureProperty {
			std::string Name;
			iw::ref<Texture> Texture;
			bool Active;
		};

		iw::linear_allocator m_alloc;
		std::vector<MaterialProperty> m_properties;
		std::vector<TextureProperty>  m_textures;

		std::unordered_map<std::string, unsigned> m_index; // also done in pipeline seems bad

		bool m_initialized;

	public:
		IWGRAPHICS_API
		Material();

		IWGRAPHICS_API
		Material(
			iw::ref<iw::Shader>& shader);

		IWGRAPHICS_API
		void Initialize(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void Use(
			const iw::ref<IDevice>& device);

		IWGRAPHICS_API
		void SetShader(
			iw::ref<iw::Shader>& shader);

#define MAT_SET(d)               \
		IWGRAPHICS_API         \
		void Set(              \
			std::string name, \
			d data);          \

		MAT_SET(bool)
		MAT_SET(int)
		MAT_SET(unsigned)
		MAT_SET(float)
		MAT_SET(double)

#undef MAT_SET

#define MAT_SET(d, s, c)             \
		IWGRAPHICS_API             \
		void Set(                  \
			std::string name,     \
			d data,               \
			unsigned stride = s,  \
			unsigned count  = c); \

		MAT_SET(bool*,       1, 1)
		MAT_SET(int*,        1, 1)
		MAT_SET(unsigned*,   1, 1)
		MAT_SET(float*,      1, 1)
		MAT_SET(double*,     1, 1)
		MAT_SET(iw::vector2, 2, 1)
		MAT_SET(iw::vector3, 3, 1)
		MAT_SET(iw::vector4, 4, 1)
		MAT_SET(Color,       4, 1)

#undef MAT_SET

		template<
			typename _t>
		_t* Get(
			std::string name)
		{
			if (!Has(name)) {
				LOG_WARNING << "Tried to get property that doesnt exist: " << name;
				return nullptr;
			}

			return (_t*)GetProperty(name).Data;
		}

		IWGRAPHICS_API
		void SetTexture(
			std::string name,
			iw::ref<Texture> texture);

		IWGRAPHICS_API
		iw::ref<Texture> GetTexture(
			std::string name);

		IWGRAPHICS_API
		bool Has(
			std::string name) const;
	private:
		IWGRAPHICS_API
		void SetProperty(
			std::string name,
			const void* data,
			UniformType type,
			unsigned typeSize,
			unsigned stride,
			unsigned count);

		IWGRAPHICS_API
		MaterialProperty& GetProperty(
			std::string name);
	};
}

	using namespace Graphics;
}
