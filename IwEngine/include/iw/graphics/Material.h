#pragma once

#include "IwGraphics.h"
#include "iw/renderer/Pipeline.h"
#include "iw/renderer/Device.h"
#include "iw/util/memory/smart_pointers.h"
#include "iw/util/tuple/foreach.h"
#include <vector>

namespace IW {
inline namespace Graphics {
	struct MaterialProperty {
		const char* Name;
		void* Data;
		IW::PipelineParamType Type;
	};

	class IWGRAPHICS_API Material {
	public:
		iwu::ref<IW::IPipeline> Pipeline;

	private:
		std::vector<MaterialProperty> m_properties;

	public:
		Material();

		Material(
			iwu::ref<IW::IPipeline>& pipline);

		~Material();

		void Use(
			const iwu::ref<IW::IDevice>& device) const;

		template<
			typename _p>
		void SetProperty(
			const char* name,
			const _p& value)
		{
			MaterialProperty* property = nullptr;
			for (MaterialProperty& p : m_properties) {
				if (p.Name == name) {
					property = &p;
					break;
				}
			}

			if (property) {
				*(_p*)property->Data = value;
			}

			else {
				MaterialProperty property{
					name,
					malloc(sizeof(_p)),
					IW::GetTypeOfParam<_p>()
				};

				*(_p*)property.Data = value;

				m_properties.push_back(property);
			}
		}
	};
}
}
