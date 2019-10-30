#include "iw/graphics/Material.h"

namespace IW {
	Material::Material()
		: Pipeline(nullptr)
	{}

	Material::Material(
		iwu::ref<IW::IPipeline>& pipeline)
		: Pipeline(pipeline)
	{}

	Material::~Material() {
		for (BufferData& bufData : m_buffers) {
			free(bufData.Buffer.Data);
		}
	}

	Color& Material::GetColor(
		const char* name)
	{
		return FindName(name, m_colors)->Color;
	}

	//void Material::SetScalar(
	//	const char* name, 
	//	Scalar scalar)
	//{
	//	auto itr = FindName(name, m_scalars);
	//	if (itr == m_scalars.end()) {
	//		m_scalars.emplace_back(name, scalar);
	//	}

	//	else {
	//		itr->Value. = scalar;
	//	}
	//}

	//void Material::SetBuffer(
	//	const char* name,
	//	size_t size,
	//	Buffer buffer)
	//{
	//	auto itr = FindName(name, m_buffers);
	//	if (itr == m_buffers.end()) {
	//		m_buffers.emplace_back(name, size, malloc(size));
	//	}

	//	else {
	//		free(itr->Buffer.Data);
	//		itr->Buffer.Data = malloc(size);
	//		memcpy(itr->Buffer.Data, buffer.Data, size);
	//	}
	//}

	void Material::SetColor(
		const char* name, 
		Color color)
	{
		auto itr = FindName(name, m_colors);
		if (itr == m_colors.end()) {
			m_colors.push_back({ name, color });
		}

		else {
			itr->Color = color;
		}
	}

	void Material::Use(
		const iwu::ref<IW::IDevice>& device) const 
	{
		device->SetPipeline(Pipeline.get());

		// this code is kinda anoyying

		IW::IPipelineParam* param;
		//for (const ScalarData& scalar : m_scalars) {
		//	param = Pipeline->GetParam(scalar.Name);

		//	if (!param) {
		//		LOG_WARNING << "Invalid property in material: " << scalar.Name;
		//		continue;
		//	}

		//	param->SetAsFloat(scalar.Value.Float);
		//}

		for (const ColorData& color : m_colors) {
			param = Pipeline->GetParam(color.Name.c_str());

			if (!param) {
				LOG_WARNING << "Invalid property in material: " << color.Name;
				continue;
			}

			param->SetAsVec4(color.Color);
		}
	}
}
