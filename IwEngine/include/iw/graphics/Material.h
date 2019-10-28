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

		void SetProperty(
			const char* name,
			PipelineParamType type,
			const void* data,
			size_t size);

		void Use(
			const iwu::ref<IW::IDevice>& device) const;

		template<
			typename _p>
		void SetProperty(
			const char* name,
			const _p& value)
		{
			SetProperty(name, IW::GetSizeOfType<_p>(), malloc(sizeof(_p)), sizeof(_p));
		}
	};
}
}
