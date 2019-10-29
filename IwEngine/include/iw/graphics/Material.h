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

namespace IW {
inline namespace Graphics {
	class IWGRAPHICS_API Material {
	public:
		iwu::ref<IW::IPipeline> Pipeline;

		enum DataType {
			Double,
			Float,
			Int
		};

		union Scalar {
			double Double;
			float  Float;
			int    Int;
		};

		union Buffer {
			char* String;
			void* Data;
		};
	private:
		// this is garbo but a lil more time on it will make it clean

		struct ScalarData {
			const char* Name;
			DataType Type; 
			Scalar Value;
		};

		struct BufferData {
			const char* Name;
			DataType Type;
			size_t Size;
			Buffer Buffer;
		};

		struct ColorData {
			const char* Name;
			Color Color;
		};

		struct TextureData {
			const char* Name;
			//IW::Texture Texture;
		};

		std::vector<ScalarData>  m_scalars;
		std::vector<BufferData>  m_buffers;
		std::vector<ColorData>   m_colors;
		std::vector<TextureData> m_textures;

	public:
		Material();

		Material(
			iwu::ref<IW::IPipeline>& pipline);

		~Material();

		void SetColor(
			const char* name,
			Color color);

		/*void SetTexture(
			const char* name,
			IW::Testure texture);*/

		void Use(
			const iwu::ref<IW::IDevice>& device) const;
	private:
		template<
			typename _t>
		typename std::vector<_t>::iterator FindName(
			const char* name,
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
