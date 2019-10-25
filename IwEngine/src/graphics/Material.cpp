#include "iw/graphics/Material.h"

namespace IW {
	Material::Material()
		: m_pipeline(nullptr)
	{}

	Material::Material(
		IW::IPipeline* pipeline)
		: m_pipeline(pipeline)
	{}

	Material::~Material() {
		for (MaterialProperty& property : m_properties) {
			free(property.Data);
		}
	}

	void Material::Use(
		const iwu::ref<IW::IDevice>& device)
	{
		device->SetPipeline(m_pipeline);
		
		for (MaterialProperty& property : m_properties) {
			IW::IPipelineParam* param = m_pipeline->GetParam(property.Name);

			if (!param) {
				LOG_WARNING << "Invalid property in material: " << property.Name;
				return;
			}

			switch (property.Type) {
				case IW::FLOAT: param->SetAsFloat(*(float*       )property.Data); break;
				case IW::VEC2:  param->SetAsVec2 (*(iwm::vector2*)property.Data); break;
				case IW::VEC3:  param->SetAsVec3 (*(iwm::vector3*)property.Data); break;
				case IW::VEC4:  param->SetAsVec4 (*(iwm::vector4*)property.Data); break;
				case IW::MAT2:  param->SetAsMat2 (*(iwm::matrix2*)property.Data); break;
				case IW::MAT3:  param->SetAsMat3 (*(iwm::matrix3*)property.Data); break;
				case IW::MAT4:  param->SetAsMat4 (*(iwm::matrix4*)property.Data); break;
			}
		}
	}
}
