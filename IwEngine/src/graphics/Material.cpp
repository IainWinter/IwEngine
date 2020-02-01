#include "iw/graphics/Material.h"
#include "iw/log/logger.h"

namespace IW {
	Material::Material()
		: m_alloc(128)
		, m_initialized(false)
	{}
	
	Material::Material(
		iw::ref<IW::Shader>& shader)
		: Shader(shader)
		, m_alloc(128)
		, m_initialized(false)
	{}

	void Material::Initialize(
		const iw::ref<IDevice>& device)
	{
		if(!m_initialized) {
			m_initialized = true;

			if (!Shader) {
				LOG_WARNING << "Tried to initialize material without a shader!";
				return;
			}
		
			if (!Shader->Program) {
				Shader->Initialize(device);
			}

			for (TextureProperty& prop : m_textures) {
				if (prop.Texture && !prop.Texture->Handle()) {
					prop.Texture->Initialize(device);
				}
			}

			int count = Shader->Program->UniformCount();
			for (int i = 0; i < count; i++) {
				IPipelineParam* uniform = Shader->Program->GetParam(i);
				if (uniform->Name().substr(0, 4) == "mat_") {
					std::string name = uniform->Name().substr(4);

					if (!Has(name)) {
						if (uniform->Type() == UniformType::SAMPLE2) {
							SetTexture(name, nullptr);
						}

						else {
							SetProperty(name, nullptr,
								uniform->Type(),
								uniform->TypeSize(),
								uniform->Stride(),
								uniform->Count());
						}
					}
				}
			}
		}
	}

	void Material::Use(
		const iw::ref<IDevice>& device)
	{
		device->SetPipeline(Shader->Program.get());

		for (MaterialProperty& prop : m_properties) {
			if (!prop.Active) {
				continue;
			}

			IPipelineParam* param = Shader->Program->GetParam("mat_" + prop.Name);

			if (!param) {
				LOG_WARNING << "Invalid property in material: " << prop.Name;
				prop.Active = false;
				continue;
			}

			switch (prop.Type) {
				case UniformType::BOOL:   param->SetAsBools  (prop.Data, prop.Count, prop.Stride);  break;
				case UniformType::INT:    param->SetAsInts   (prop.Data, prop.Count, prop.Stride);  break;
				case UniformType::UINT:   param->SetAsUInts  (prop.Data, prop.Count, prop.Stride);  break;
				case UniformType::FLOAT:  param->SetAsFloats (prop.Data, prop.Count, prop.Stride);  break;
				case UniformType::DOUBLE: param->SetAsDoubles(prop.Data, prop.Count, prop.Stride);  break;
				default: LOG_WARNING << "Invalid property in material: " << prop.Name; break;
			}
		}

		for (TextureProperty& prop : m_textures) {
			if (!prop.Active) {
				continue;
			}

			IPipelineParam* param = Shader->Program->GetParam("mat_" + prop.Name);

			if (!param) {
				LOG_WARNING << "Invalid texture in material: " << prop.Name;
				prop.Active = false;
				continue;
			}

			if (prop.Texture) {
				if (!prop.Texture->Handle()) {
					LOG_WARNING << "Uninitialized texture in material: " << prop.Name;
				}

				param->SetAsTexture(prop.Texture->Handle());
			}

			else {
				param->SetAsTexture(nullptr);
			}
		}
	}

	void Material::SetShader(
		iw::ref<IW::Shader>& shader)
	{
		Shader = shader;
	}

#define MAT_SETS(d, ut)                                    \
		void Material::Set(                                \
			std::string name,                              \
			d data)                                        \
		{                                                  \
			SetProperty(name, &data, ut, sizeof(d), 1, 1); \
		}                                                  \

	MAT_SETS(bool,     UniformType::BOOL)
	MAT_SETS(int,      UniformType::INT)
	MAT_SETS(unsigned, UniformType::UINT)
	MAT_SETS(float,    UniformType::FLOAT)
	MAT_SETS(double,   UniformType::DOUBLE)

#undef MAT_SETS

#define MAT_SET(d, ut, ts, mp)                                  \
		void Material::Set(                                     \
			std::string name,                                   \
			d data,                                             \
			unsigned stride,                                    \
			unsigned count)                                     \
		{                                                       \
			SetProperty(name, mp(data), ut, ts, stride, count); \
		}                                                       \

		MAT_SET(bool*,       UniformType::BOOL,   sizeof(bool))
		MAT_SET(int*,        UniformType::INT,    sizeof(int))
		MAT_SET(unsigned*,   UniformType::UINT,   sizeof(unsigned))
		MAT_SET(float*,      UniformType::FLOAT,  sizeof(float))
		MAT_SET(double*,     UniformType::DOUBLE, sizeof(double))
		MAT_SET(iw::vector2, UniformType::FLOAT,  sizeof(float), &)
		MAT_SET(iw::vector3, UniformType::FLOAT,  sizeof(float), &)
		MAT_SET(iw::vector4, UniformType::FLOAT,  sizeof(float), &)
		MAT_SET(Color,       UniformType::FLOAT,  sizeof(float), &)

#undef MAT_SET

	void Material::SetTexture(
		std::string name,
		iw::ref<Texture> texture)
	{
		if (Has(name)) {
			TextureProperty& prop = m_textures.at(m_index.at(name));
			prop.Texture = texture;
			prop.Active = true;
		}

		else {
			auto itr = m_index.emplace(name, m_textures.size());

			TextureProperty prop {
				itr.first->first,
				texture,
				true
			};

			m_textures.push_back(prop);
		}

		std::string n(name);

		name[0] = toupper(name[0]);
		Set("has" + name, (float)(texture != nullptr));
	}

	iw::ref<Texture> Material::GetTexture(
		std::string name)
	{
		if (!Has(name)) {
			LOG_WARNING << "Tried to get texture that doesnt exist: " << name;
			return nullptr;
		}

		return m_textures.at(m_index.at(name)).Texture;
	}

	bool Material::Has(
		std::string name) const
	{
		return m_index.find(name) != m_index.end();
	}

	void Material::SetProperty(
		std::string name,
		const void* data,
		UniformType type,
		unsigned typeSize,
		unsigned stride,
		unsigned count)

	{
		if (type == UniformType::UNKNOWN) {
			LOG_WARNING << "Attempted to set uniform with an invalid type: " << name << "!";
			return;
		}

		unsigned size = typeSize * stride * count;

		MaterialProperty* prop;
		if (Has(name)) {
			prop = &m_properties.at(m_index.at(name));
			if (prop->Type != type) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different type: " << name << "!";
				return;
			}
			
			if (prop->TypeSize != typeSize) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different type size: " << name << "!";
				return;
			}

			// may want to allow smaller counts and strides but lets see 
			if (prop->Stride != stride) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different stride: " << name << "!";
				return;
			}

			if (prop->Count != count) {
				LOG_WARNING << "Attempted to set uniform with duplicate name but different count: " << name << "!";
				return;
			}

			prop->Active = true;
		}

		else {
			auto itr = m_index.emplace(name, m_properties.size());

			prop = &m_properties.emplace_back(
				MaterialProperty {
					itr.first->first,
					m_alloc.alloc(size),
					type,
					typeSize,
					stride,
					count,
					true
				}
			);

			if (!prop->Data) {
				m_alloc.resize(m_alloc.capacity() + size);
				prop->Data = m_alloc.alloc(size);
			}
		}

		if (!data) {
			memset(prop->Data, 0, size);
		}

		else {
			memcpy(prop->Data, data, size);
		}
	}

	Material::MaterialProperty& Material::GetProperty(
		std::string name)
	{
		return m_properties.at(m_index.at(name));
	}
}
