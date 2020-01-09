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

	void Material::Initialize(
		const iw::ref<IDevice>& device)
	{
		if (Shader) {
			LOG_WARNING << "Tried to initialize material without a shader!";
			return;
		}
		
		if (!Shader->Program) {
			Shader->Initialize(device);
		}

		int count = Shader->Program->UniformCount();
		for (int i = 0; i < count; i++) {
			IPipelineParam* uniform = Shader->Program->GetParam(i);
			if (uniform->Name().substr(0, 4) == "mat_") {
				const char* name = uniform->Name().substr(4).c_str();
				unsigned    c    = uniform->Size() / uniform->TypeSize();
				switch (uniform->Type()) {
					case UniformType::BOOL:    SetBools  (name, nullptr, c, uniform->Size()); break;
					case UniformType::INT:     SetInts   (name, nullptr, c, uniform->Size()); break;
					case UniformType::UINT:    SetUInts  (name, nullptr, c, uniform->Size()); break;
					case UniformType::FLOAT:   SetFloats (name, nullptr, c, uniform->Size()); break;
					case UniformType::DOUBLE:  SetDoubles(name, nullptr, c, uniform->Size()); break;
					case UniformType::SAMPLE2: SetTexture(name, nullptr); break;
				}
			}
		}
	}

	void Material::Use(
		const iw::ref<IDevice>& device) const
	{
		device->SetPipeline(Shader->Program.get());

		for (const MaterialProperty& prop : m_properties) {
			IPipelineParam* param = Shader->Program->GetParam(prop.Name);

			if (!param) {
				LOG_WARNING << "Invalid property in material: " << prop.Name;
				continue;
			}

			if (prop.IsSample) {
				Texture* texture = (Texture*)prop.Data;
				if (!texture) {
					param->SetAsTexture(nullptr);
				}
				
				else if (!texture->Handle()) {
					LOG_WARNING << "Texture not initialized: " << prop.Name;
				}

				else {
					param->SetAsTexture(texture->Handle());
				}
			}

			else {
				switch (prop.Type) {
					case UniformType::BOOL:   param->SetAsBools  (prop.Data, prop.Count, prop.Stride);  break;
					case UniformType::INT:    param->SetAsInts   (prop.Data, prop.Count, prop.Stride);  break;
					case UniformType::UINT:   param->SetAsUInts  (prop.Data, prop.Count, prop.Stride);  break;
					case UniformType::FLOAT:  param->SetAsFloats (prop.Data, prop.Count, prop.Stride);  break;
					case UniformType::DOUBLE: param->SetAsDoubles(prop.Data, prop.Count, prop.Stride);  break;
					default: LOG_WARNING << "Invalid property in material: " << prop.Name; break;
				}
			}
		}
	}

	bool Material::HasProperty(
		const char* name) /*const*/
	{
		return FindProperty(name) != nullptr;
	}

	void Material::SetShader(
		iw::ref<IW::Shader>& shader)
	{
		Shader = shader;
	}

	void Material::SetBool(
		const char* name,
		bool value)
	{
		CreateProperty(name, &value, 1, 0, false, UniformType::BOOL, sizeof(bool));
	}

	void Material::SetBools(
		const char* name,
		void* values,
		unsigned count,
		unsigned stride)
	{
		CreateProperty(name, values, count, stride, false, UniformType::BOOL, sizeof(bool));
	}

	void Material::SetInt(
		const char* name,
		int value)
	{
		CreateProperty(name, &value, 1, 0, false, UniformType::INT, sizeof(int));
	}

	void Material::SetInts(
		const char* name,
		void* values,
		unsigned count,
		unsigned stride)
	{
		CreateProperty(name, values, count, stride, false, UniformType::INT, sizeof(int));
	}

	void Material::SetUInt(
		const char* name,
		unsigned int value)
	{
		CreateProperty(name, &value, 1, 0, false, UniformType::UINT, sizeof(unsigned int));
	}

	void Material::SetUInts(
		const char* name,
		void* values,
		unsigned count,
		unsigned stride)
	{
		CreateProperty(name, values, count, stride, false, UniformType::UINT, sizeof(unsigned int));
	}

	void Material::SetFloat(
		const char* name,
		float value)
	{
		CreateProperty(name, &value, 1, 0, false, UniformType::FLOAT, sizeof(float));
	}

	void Material::SetFloats(
		const char* name,
		void* values,
		unsigned count,
		unsigned stride)
	{
		CreateProperty(name, values, count, stride, false, UniformType::FLOAT, sizeof(float));
	}

	void Material::SetDouble(
		const char* name,
		double value)
	{
		CreateProperty(name, &value, 1, 0, false, UniformType::DOUBLE, sizeof(double));
	}

	void Material::SetDoubles(
		const char* name,
		void* values,
		unsigned count,
		unsigned stride)
	{
		CreateProperty(name, values, count, stride, false, UniformType::DOUBLE, sizeof(double));
	}

	void Material::SetTexture(
		const char* name, 
		Texture* texture)
	{
		CreateProperty(name, texture, 0, 0, true, UniformType::SAMPLE2, 0);
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
		for (MaterialProperty& p : m_properties) {
			if (strcmp(p.Name, name) == 0) {
				prop = &p;
			}
		}

		return prop;
	}

	void Material::CreateProperty(
		const char* name,
		void* values,
		unsigned count,
		unsigned stride,
		bool isSample,
		UniformType type,
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
