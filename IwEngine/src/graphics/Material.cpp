#include "iw/graphics/Material.h"

namespace IW {
	Material::Material(
		iw::ref<IW::Shader>& shader)
		: Shader(shader)
	{}

	Material::Material(
		const Material& copy)
		: Shader(copy.Shader)
	{
		for (const MaterialProperty& prop : copy.m_properties) {
			size_t nameSize = strlen(prop.Name) + 1;

			MaterialProperty& newProp = m_properties.emplace_back(
				MaterialProperty{
					(char*)malloc(nameSize),
					prop.Size,
					prop.Count,
					prop.Stride,
					prop.IsSample,
					prop.Type,
					malloc(prop.Size)
				}
			);

			memcpy(newProp.Name, prop.Name, nameSize);
			memcpy(newProp.Data, prop.Data, prop.Size);
		}
	}

	Material::~Material() {
		for (MaterialProperty& prop : m_properties) {
			delete[] prop.Name;
			free(prop.Data);
		}
	}

	Material& Material::operator=(
		const Material& copy)
	{
		Shader = copy.Shader;
		for (const MaterialProperty& prop : copy.m_properties) {
			size_t nameSize = strlen(prop.Name) + 1;

			MaterialProperty& newProp = m_properties.emplace_back(
				MaterialProperty{
					(char*)malloc(nameSize),
					prop.Size,
					prop.Count,
					prop.Stride,
					prop.IsSample,
					prop.Type,
					malloc(prop.Size)
				}
			);

			memcpy(newProp.Name, prop.Name, nameSize);
			memcpy(newProp.Data, prop.Data, prop.Size);
		}

		return *this;
	}

	void Material::SetBool(
		const char* name,
		bool value)
	{
		CreateProperty(name, &value, 1, 0, false, BOOL, sizeof(bool));
	}

	void Material::SetBools(
		const char* name,
		void* values,
		size_t count,
		size_t stride)
	{
		CreateProperty(name, values, count, stride, false, BOOL, sizeof(bool));
	}

	void Material::SetInt(
		const char* name,
		int value)
	{
		CreateProperty(name, &value, 1, 0, false, INT, sizeof(int));
	}

	void Material::SetInts(
		const char* name,
		void* values,
		size_t count,
		size_t stride)
	{
		CreateProperty(name, values, count, stride, false, INT, sizeof(int));
	}

	void Material::SetUInt(
		const char* name,
		unsigned int value)
	{
		CreateProperty(name, &value, 1, 0, false, UINT, sizeof(unsigned int));
	}

	void Material::SetUInts(
		const char* name,
		void* values,
		size_t count,
		size_t stride)
	{
		CreateProperty(name, values, count, stride, false, UINT, sizeof(unsigned int));
	}

	void Material::SetFloat(
		const char* name,
		float value)
	{
		CreateProperty(name, &value, 1, 0, false, FLOAT, sizeof(float));
	}

	void Material::SetFloats(
		const char* name,
		void* values,
		size_t count,
		size_t stride)
	{
		CreateProperty(name, values, count, stride, false, FLOAT, sizeof(float));
	}

	void Material::SetDouble(
		const char* name,
		double value)
	{
		CreateProperty(name, &value, 1, 0, false, DOUBLE, sizeof(double));
	}

	void Material::SetDoubles(
		const char* name,
		void* values,
		size_t count,
		size_t stride)
	{
		CreateProperty(name, values, count, stride, false, DOUBLE, sizeof(double));
	}

	void Material::SetTexture(
		const char* name, 
		Texture* texture)
	{
		CreateProperty(name, texture, 0, 0, true, SAMPLE, 0);
	}

	void Material::SetTexture(
		const char* name,
		const iw::ref<Texture>& texture)
	{
		SetTexture(name, texture.get());
	}

	bool* Material::GetBool(
		const char* name)
	{
		return (bool*)std::get<0>(GetData(name));
	}

	std::tuple<bool*, size_t> Material::GetBools(
		const char* name)
	{
		auto data = GetData(name);
		return { (bool*)std::get<0>(data), std::get<1>(data) };
	}

	int* Material::GetInt(
		const char* name)
	{
		return (int*)std::get<0>(GetData(name));
	}

	std::tuple<int*, size_t> Material::GetInts(
		const char* name)
	{
		auto data = GetData(name);
		return { (int*)std::get<0>(data), std::get<1>(data) };
	}

	unsigned int* Material::GetUInt(
		const char* name)
	{
		return (unsigned int*)std::get<0>(GetData(name));
	}

	std::tuple<unsigned int*, size_t> Material::GetUInts(
		const char* name)
	{
		auto data = GetData(name);
		return { (unsigned int*)std::get<0>(data), std::get<1>(data) };
	}

	float* Material::GetFloat(
		const char* name)
	{
		return (float*)std::get<0>(GetData(name));
	}

	std::tuple<float*, size_t> Material::GetFloats(
		const char* name)
	{
		auto data = GetData(name);
		return { (float*)std::get<0>(data), std::get<1>(data) };
	}

	double* Material::GetDouble(
		const char* name)
	{
		return (double*)std::get<0>(GetData(name));
	}

	std::tuple<double*, size_t> Material::GetDoubles(
		const char* name)
	{
		auto data = GetData(name);
		return { (double*)std::get<0>(data), std::get<1>(data) };
	}

	Texture* Material::GetTexture(
		const char* name)
	{
		return (Texture*)std::get<0>(GetData(name));
	}

	void Material::Use(
		const iw::ref<IDevice>& device) const
	{
		device->SetPipeline(Shader->Program);

		for (const MaterialProperty& prop : m_properties) {
			IPipelineParam* param = Shader->Program->GetParam(prop.Name);

			if (!param) {
				LOG_WARNING << "Invalid property in material: " << prop.Name;
				continue;
			}

			if (prop.IsSample) {
				Texture* texture = (Texture*)prop.Data;
				param->SetAsTexture(texture->Handle);
			}

			else {
				switch (prop.Type) {
					case BOOL:   param->SetAsBools  (prop.Data, prop.Count, prop.Stride); break;
					case INT:    param->SetAsInts   (prop.Data, prop.Count, prop.Stride); break;
					case UINT:   param->SetAsUInts  (prop.Data, prop.Count, prop.Stride); break;
					case FLOAT:  param->SetAsFloats (prop.Data, prop.Count, prop.Stride); break;
					case DOUBLE: param->SetAsDoubles(prop.Data, prop.Count, prop.Stride); break;
					default: LOG_WARNING << "Invalid property in material: " << prop.Name; break;
				}
			}
		}
	}

	std::tuple<void*, size_t> Material::GetData(
		const char* name)
	{
		MaterialProperty* prop = FindProperty(name);
		return { prop ? (bool*)prop->Data : nullptr, prop ? prop->Count : 0 };
	}

	Material::MaterialProperty* Material::FindProperty(
		const char* name) 
	{
		MaterialProperty* prop = nullptr;
		for (MaterialProperty& prop : m_properties) {
			if (strcmp(prop.Name, name) == 0) {
				return &prop;
			}
		}

		return prop;
	}

	void Material::CreateProperty(
		const char* name,
		void* values,
		size_t count,
		size_t stride,
		bool isSample,
		MaterialPropertyType type,
		size_t typeSize)
	{
		MaterialProperty* prop = FindProperty(name);
		size_t size = count * typeSize;

		if (prop) {
			if (prop->IsSample) {
				free(prop->Data);
			}

			prop->Size = size;
			prop->Count = count;
			prop->Stride = stride;
			prop->IsSample = isSample;
			prop->Type = type;

			if (isSample) {
				prop->Data = values;
			}

			else {
				prop->Data = malloc(size);
				memcpy(prop->Data, values, size);
			}
		}

		else {
			size_t nameSize = strlen(name) + 1;

			prop = &m_properties.emplace_back(
				MaterialProperty{
					(char*)malloc(nameSize),
					size,
					count,
					stride,
					isSample,
					type,
					isSample ? values : malloc(size)
				}
			);

			if (!isSample) {
				memcpy(prop->Data, values, size);
			}

			memcpy(prop->Name, name, nameSize);
		}
	}
}
